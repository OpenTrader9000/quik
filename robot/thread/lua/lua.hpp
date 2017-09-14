#pragma once


extern "C" {
#include <lauxlib.h>
}

#include <memory>


#include <common/message/message.hpp>
#include <common/defines.hpp>
#include <sol/sol.hpp>


namespace robot {
namespace thread {
namespace lua {

void work(lua_State* L);
void start(lua_State* L);
void stop(lua_State* L);

void enqueue_task(common::message::ptr task);

struct lua {

    lua();
    ~lua();

    void work(lua_State* L);
    void start(lua_State* L);
    void stop(lua_State* L);

    void dump(char const* function, sol::table tab);

    void on_trade(sol::table trade);
    void on_transaction(sol::table transaction);
    void on_quote(char const* class_code, char const* sec_code, sol::table tab);
    void setup_scenario(char const* name);
    void dump_scenario(char const* name, sol::table, char const* info = ""); // default is functions

    uint64_t                          last_ts_in_ms_;
    std::vector<common::message::ptr> scenario_cache_;

   // private:
    common::multithread_queue_t exec_queue_;

    static std::unique_ptr<lua> instance_;

};

} // namespace lua

} // namespace thread
} // namespace robot