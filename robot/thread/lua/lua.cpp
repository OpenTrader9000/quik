#include "lua.hpp"
#include "details/deserialize.hpp"
#include "details/datetime.hpp"
#include <common/message/event/stop.hpp>
#include <message/lua/exec.hpp>
#include <sol/sol.hpp>
#include <worker/config.hpp>
#include <worker/dispatcher.hpp>
#include <worker/run.hpp>
#include <common/message/general/unhandled.hpp>
#include <common/message/scenario/entry.hpp>
#include <common/message/event/compress.hpp>
#include <common/message/event/flush.hpp>
#include <common/scenario/scenario.hpp>
#include <persistent/persistent.hpp>
#include <utils/time/time.hpp>
#include <common/message/trade/trade.hpp>
#include <common/message/trade/quotes.hpp>



namespace robot {
namespace thread {
namespace lua {

std::unique_ptr<lua> lua::instance_;

lua::lua()
: is_test_(false)
, day_end_timestamp_(0)
, send_timestamp_(utils::time::timestamp_in_ms()) {}

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
    .set_function("on_clean_up",  [this]() {on_clean_up(); })
    .set_function("allow_parse_order_book", [this] (char const* sec_code) { return allow_parse_order_book(sec_code); })
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
    auto persistent                           = config_table["persistent"];
    config.persistent_.path2db_               = persistent["path2db"];
    config.persistent_.trades_archive_folder_ = persistent["trades_archive"];

    /*LOAD LOG CONFIG*/
    auto log          = config_table["log"];
    config.log_.path_ = log["path"];
    config.log_.parse_level(log["level"]);

    sol::table instruments = config_table["instruments"];
    instruments.for_each([&](sol::object const&, sol::object const& value) {
        sol::table inst = value;
        std::string sec_code = inst["sec_code"];
        int         power    = inst["pow"];

        pows_.emplace(sec_code, power);
        order_books_history_.emplace(sec_code, power);
    });

    worker::dispatcher::init(config);
}


// dump some function single input
void lua::dump(char const* function, sol::table tab) {

    update_timestamp();
    check_new_day();
    
    using unhandled_t = common::message::general::unhandled;

    auto mes = common::message::make<unhandled_t>();

    mes->set_type_function();
    mes->name() = function;
    mes->timestamp() = last_timestamp_;

    details::deserialize(tab, mes);

    query_cache_.push_back(std::move(mes));

    //persistent::push_query(std::move(mes));
}

void lua::setup_scenario(char const* name) {
    int scenario_id = persistent::create_scenario(name);
    common::scenario::set(scenario_id);
}


//void lua::dump_scenario(char const * name, sol::table tab, char const* info )
//{
//    using scenario_entry_t = common::message::scenario::entry;
//
//    update_timestamp();
//    check_new_day();
//
//    auto mes = common::message::make<scenario_entry_t>();
//
//    mes->set_type_function();
//    mes->name()        = name;
//    mes->timestamp()   = last_timestamp_;
//    mes->info()        = info;
//    mes->scenario_id() = common::scenario::id();
//
//    details::deserialize(tab, mes);
//
//    scenario_cache_.push_back(std::move(mes));
//}

void lua::update_timestamp() {

    // if test is running timestamp must be setting up by setup_test_timestamp
    if (is_test_)
        return;

    // this small optimization avoid double time computation
    // all callbacks runs in the single one thread
    last_timestamp_ = utils::time::timestamp_in_ms();
}

/*
    try push all message to persistent
*/
void lua::try_flush()
{
    static constexpr unsigned send_periond_in_ms = 1000;

    // send data every n ms
    if (last_timestamp_ - send_timestamp_ > send_periond_in_ms) {
        flush();
        send_timestamp_ = last_timestamp_;
    }
}

void lua::flush() {
    persistent::push_queries(query_cache_);
    persistent::push_trades(trade_cache_);
	persistent::push_quotes(quote_cache_);
    query_cache_.clear();
    trade_cache_.clear();
	quote_cache_.clear();
 }

 void lua::check_new_day() {

     namespace mes = common::message;

     if (last_timestamp_ > day_end_timestamp_) { // very unlikely

         //TODO: make functions for all this time
         uint64_t day_in_ms = 3600 * 24 * 1000;
         day_end_timestamp_ = (last_timestamp_ - last_timestamp_ % day_in_ms) + day_in_ms;
         clear_order_books();

         // flush all previous data
         flush();

         auto flush1 = mes::make<mes::event::flush>();
         auto flush2 = mes::make<mes::event::flush>();
         persistent::push_trade(std::move(flush1));
         persistent::push_quote(std::move(flush2));


         auto mes1 = mes::make<mes::event::compress>();
         auto mes2 = mes::make<mes::event::compress>();
         persistent::push_trade(std::move(mes1));
         persistent::push_quote(std::move(mes2));
     }
 }

 void lua::clear_order_books() {

     // generate messages for flushing
     for (auto& sec_code_ob : order_books_history_) {

         auto const& ob = sec_code_ob.second.current();

         // empty order book
         if (ob.empty())
             continue;

         auto q_ptr = common::message::make<common::message::trade::quotes>();
         q_ptr->sec_code_ = sec_code_ob.first;

         // zero order book in file
         for (auto& bid : ob.bid_) {
             common::storage::quote q;
             q.set_bid();
             q.price_ = bid.price_;
             q.quantity_diff_ = -bid.quantity_;
             q.machine_timestamp_ = ob.timestamp_ + 1; // timestamp must differs
             q_ptr->quotes_.push_back(q);
         }

         for (auto& offer : ob.offer_) {
             common::storage::quote q;
             q.set_offer();
             q.price_ = offer.price_;
             q.quantity_diff_ = -offer.quantity_;
             q.machine_timestamp_ = ob.timestamp_ + 1;// timestamp must differs
             q_ptr->quotes_.push_back(q);
         }

         quote_cache_.push_back(q_ptr);
     }

     // clear all order books
     for (auto& sec_order_book : order_books_history_) {
         sec_order_book.second.clear();
     }
 }

 bool lua::allow_parse_order_book(char const * sec_code)
 {
     return order_books_history_.find(sec_code) != order_books_history_.end();
 }

 void lua::on_trade(sol::table trade) {

     update_timestamp();
     check_new_day();

     // auto dum = stackDump(trade.lua_state());

     //if (common::scenario::id() != 0) {
     //    dump_scenario("OnAllTrade", trade);
     //}

     std::string sec_code = trade["sec_code"];

     auto message                 = common::message::make<common::message::trade::trade>();
     message->price()             = common::numbers::bcd(trade["price"], pows_[sec_code]);
     message->server_timestamp()  = details::timestamp(trade["datetime"]);
     message->sec_code_           = trade["sec_code"];
     message->open_interest()     = trade["open_interest"];
     message->quantity()          = trade["qty"];
     message->trade_number()      = trade["trade_num"];
     message->machine_timestamp() = last_timestamp_;

     // message->price().parse((char*)trade["price"]);

     // parse bid
     int  flags    = trade["flags"];
     bool is_offer = ((flags & 1) != 0);
     bool is_bid   = ((flags & 2) != 0);
     assert(is_bid || is_offer);

     if (is_bid) {
         message->trade_.set_bid();
     } else {
         message->trade_.set_offer();
     }

     trade_cache_.push_back(std::move(message));

     try_flush();

}

void lua::on_transaction(sol::table transaction){
    update_timestamp();

    // 
    try_flush();
}

// TODO: Add timestamp for order book. 
void lua::on_quote(char const* class_code, char const* sec_code, sol::table tab){

    update_timestamp();
    check_new_day();

	auto& qdiff = order_books_history_[sec_code];
	auto message = qdiff.mkdiff(tab);

	assert(!message->quotes_.empty());

    // update lua specific info
	message->sec_code_ = sec_code;
	for (auto& q : message->quotes_)
		q.machine_timestamp_ = last_timestamp_;
    qdiff.current().timestamp_ = last_timestamp_;

	quote_cache_.push_back(message);
        
    try_flush();
}

// proper way to clear state. Also this code clears order books in files.
void lua::on_clean_up() {
    update_timestamp();
    check_new_day();

    clear_order_books();
}


void lua::stop(lua_State* L) {
    using namespace common::message;

    clear_order_books();

	flush();

    // first of all stop main loop
    exec_queue_.enqueue(make<event::stop>());

    // must flush all persistent buffers
    persistent::stop();
    
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

    // stop events from dispatcher
    worker::dispatcher::stop();
    lua::instance_->stop(L);

    //clearing
    worker::dispatcher::clear();
    lua::instance_.reset();

}

void run_in_test_environment()
{
    assert(lua::instance_);
    lua::instance_->is_test_ = true;
}

void setup_test_timestamp(uint64_t new_timestamp)
{
    assert(lua::instance_);
    lua::instance_->last_timestamp_ = new_timestamp;
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