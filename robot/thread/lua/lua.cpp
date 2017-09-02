#include "lua.hpp"
#include "details/serialize.hpp"
#include <common/message/event/stop.hpp>
#include <message/lua/exec.hpp>
#include <sol/sol.hpp>
#include <worker/config.hpp>
#include <worker/dispatcher.hpp>
#include <worker/run.hpp>
#include <common/message/general/unhandled.hpp>

namespace robot {
namespace thread {
namespace lua {

std::unique_ptr<lua> lua::instance_;

lua::lua() {
}

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
    .set_function("on_quote", [this](char const* class_code,
                                     char const* sec_code) { on_quote(class_code, sec_code); })
    .set_function("on_transaction", [this](sol::table transaction) { on_transaction(transaction); });

    sol::table config_table(L);

    /*LOAD CONFIG*/
    worker::config config;

    /* LOAD TELEGRAM CONFIG*/
    auto telegram               = config_table["telegram"];
    config.telegram_.bot_token_ = telegram["token"];
    config.telegram_.ids_       = extract_vector<int64_t>(telegram["allowed_ids"]);


    worker::dispatcher::init(config);
}


void lua::dump(char const* function, sol::table tab) {
    
    using unhandled_t = common::message::general::unhandled;

    auto mes = common::message::make<unhandled_t>();

    mes->name_ = function;

    details::deserialize(tab, mes);

}

void lua::on_trade(sol::table trade){
}

void lua::on_transaction(sol::table transaction){
}

void lua::on_quote(char const* class_code, char const* sec_code){
}


void lua::stop(lua_State* L) {
    using namespace common::message;
    exec_queue_.enqueue(make<event::stop>());
}


void work(lua_State* L) {
    lua::instance_->work(L);
}

void start(lua_State* L) {
    lua::instance_.reset(new lua());
    lua::instance_->start(L);
}

void stop(lua_State* L) {
    lua::instance_->stop(L);
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