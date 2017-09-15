#include "lua.hpp"
#include "details/deserialize.hpp"
#include <common/message/event/stop.hpp>
#include <message/lua/exec.hpp>
#include <sol/sol.hpp>
#include <worker/config.hpp>
#include <worker/dispatcher.hpp>
#include <worker/run.hpp>
#include <common/message/general/unhandled.hpp>
#include <common/message/scenario/entry.hpp>
#include <common/scenario/scenario.hpp>
#include <persistent/persistent.hpp>
#include <utils/time/time.hpp>

namespace robot {
namespace thread {
namespace lua {

std::unique_ptr<lua> lua::instance_;

lua::lua()
: last_ts_in_ms_(std::numeric_limits<decltype(last_ts_in_ms_)>::max()) 
{}

lua::~lua() {
}

void lua::work(lua_State* L) {

    using namespace common::message;

    while (worker::run()) {

        ptr task;
        exec_queue_.wait_dequeue(task);

        if (task->code_ == event::stop::code)
            return;

        task.cast<message::lua::exec>()->execute(L);
    }
}

template <typename value_type>
std::vector<value_type> extract_vector(sol::table tab) {
    std::vector<value_type> result;
    tab.for_each([&result](sol::object const&, sol::object const& value) {
        auto val = value.as<value_type>();
        result.push_back(val);
    });
    return result;
}

void lua::start(lua_State* L) {


    sol::state_view s(L);

    s.set_function("dump", [this](char const* name, sol::table tab) { dump(name, tab); })
    .set_function("on_trade", [this](sol::table trade) { on_trade(trade); })
    .set_function("is_security_handles",
                  [this](char const* class_code, char const* sec_code) { return false; })
    .set_function("on_quote", [this](char const* class_code, char const* sec_code,
                                     sol::table tab) { on_quote(class_code, sec_code, tab); })
    .set_function("on_transaction", [this](sol::table transaction) { on_transaction(transaction); })
    .set_function("setup_scenario",
                  [this](char const* scenario_name) { setup_scenario(scenario_name); });

    sol::table config_table(L);

    /*LOAD CONFIG*/
    worker::config config;

    /* LOAD TELEGRAM CONFIG*/
    auto telegram               = config_table["telegram"];
    config.telegram_.bot_token_ = telegram["token"];
    config.telegram_.ids_       = extract_vector<int64_t>(telegram["allowed_ids"]);


    /* LOAD PERSISTENT CONFIG */
    auto persistent             = config_table["persistent"];
    config.persistent_.path2db_ = persistent["path2db"];

    worker::dispatcher::init(config);
}


// dump some function single input
void lua::dump(char const* function, sol::table tab) {
    
    using unhandled_t = common::message::general::unhandled;

    auto mes = common::message::make<unhandled_t>();

    mes->set_type_function();
    mes->name() = function;
    mes->timestamp() = utils::time::timestamp_in_ms();

    details::deserialize(tab, mes);

    persistent::push_query(std::move(mes));
}

void lua::setup_scenario(char const* name) {
    int scenario_id = persistent::create_scenario(name);
    common::scenario::set(scenario_id);
}


void lua::dump_scenario(char const * name, sol::table tab, char const* info )
{
    static constexpr unsigned send_periond_in_ms = 1000;

    using scenario_entry_t = common::message::scenario::entry;

    auto mes = common::message::make<scenario_entry_t>();

    mes->set_type_function();
    mes->name() = name;
    mes->timestamp() = utils::time::timestamp_in_ms();
    mes->info() = info;
    mes->scenario_id() = common::scenario::id();

    details::deserialize(tab, mes);

    auto timestamp = mes->timestamp();

    scenario_cache_.push_back(std::move(mes));
    
    // send data every n ms
    if (last_ts_in_ms_ - timestamp > send_periond_in_ms) {
        persistent::push_queries(scenario_cache_);
        scenario_cache_.clear();
        last_ts_in_ms_ = timestamp;
    }
}

void lua::on_trade(sol::table trade){
    if (common::scenario::id() != 0) {
        dump_scenario("OnAllTrade", trade);
        return;
    }
}

void lua::on_transaction(sol::table transaction){

}

void lua::on_quote(char const* class_code, char const* sec_code, sol::table tab){
    
    if (common::scenario::id() != 0) {
        
        char buffer[65] = {};
        sprintf_s(buffer, "%s-%s", class_code, sec_code);

        dump_scenario("OnQuote", tab, buffer);
        return;
    }
}


void lua::stop(lua_State* L) {
    using namespace common::message;

    // end all queries
    if (!scenario_cache_.empty()) {
        persistent::push_queries(scenario_cache_);
        scenario_cache_.clear();
    }

    // must flush all persistent buffers
    persistent::stop();
    
    exec_queue_.enqueue(make<event::stop>());
}


void work(lua_State* L) {
    lua::instance_->work(L);
}

void start(lua_State* L) {
    lua::instance_.reset(new lua());
    lua::instance_->start(L);
}

// every possible static/global class must be cleans here
void stop(lua_State* L) {
    // stopping
    worker::dispatcher::stop();
    lua::instance_->stop(L);

    //clearing
    worker::dispatcher::clear();
    lua::instance_.reset();

}

void enqueue_task(common::message::ptr task) {
    assert(task->code_ == message::lua::exec::code);
    lua::instance_->exec_queue_.enqueue(task);
}

void clear() {
    lua::instance_.reset();
}


} // namespace lua
} // namespace thread
} // namespace robot