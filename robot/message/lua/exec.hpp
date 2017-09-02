#pragma once

extern "C" {
//#include <lauxlib.h>
struct lua_State;
}

#include <common/message/base.hpp>

namespace robot {
namespace message {
namespace lua {

struct exec : public common::message::base {

    enum { code = common::message::codes::EXEC_LUA };

    exec()
    : base(code) {
    }

    virtual ~exec() {
    }

    virtual void execute(lua_State* L) = 0;
};


template <typename FunctionType>
struct exec_wrapper : public exec {

    exec_wrapper(FunctionType&& function)
    : function_(function) {
    }

    void execute(lua_State* L) override {
        function_(L);
    }

    FunctionType function_;
};

template <typename FunctionType>
auto make_exec(FunctionType&& function) {
    using ReturnType = common::message::ptr_concrete<exec_wrapper<FunctionType>>;
    return ReturnType(new exec_wrapper<FunctionType>(std::forward<FunctionType>(function)));
}
} // namespace lua
} // namespace message
} // namespace robot