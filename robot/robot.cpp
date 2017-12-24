// tg_conroller.cpp : Defines the exported functions for the DLL application.
//

// single one dependency
#ifndef _TEST
#pragma comment (lib, "qlua.lib")
#endif


extern "C" {
#include <lauxlib.h>
}


#include <thread>
#include <fstream>

// TODO: remove
#include <sol/sol.hpp>

#include <thread/lua/lua.hpp>
#include <worker/run.hpp>

extern "C" int __declspec(dllexport) start (lua_State* L) {
    // register all possible functions
    // and init from config
    robot::thread::lua::start(L);
    return 0;
}

extern "C" int __declspec(dllexport) stop (lua_State* L) {
    robot::thread::lua::stop(L);
    return 0;
}

extern "C" int __declspec(dllexport) do_work(lua_State* L) {
    robot::thread::lua::work(L);
    return 0;
}

extern "C" void __declspec(dllexport) run_in_test_environment() {
    robot::thread::lua::run_in_test_environment();
}

extern "C" void __declspec(dllexport) setup_test_timestamp(uint64_t new_timestamp) {
    robot::thread::lua::setup_test_timestamp(new_timestamp);
}

static const luaL_Reg lib[] = { { "stop", stop }, { "start", start }, { "do_work", do_work }, { nullptr, nullptr } };

extern "C" int __declspec(dllexport) luaopen_robot(lua_State* L) {
	luaL_register(L, "robot", lib);
	return 0;
}
