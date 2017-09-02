#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>


namespace robot {namespace message {
namespace common {

struct exec : public base {

    enum { code = codes::EXEC };

    exec()
    : base(code) {
    }

    virtual ~exec() {
    }

    virtual void execute() = 0;
};

template <typename FunctionType>
struct exec_wrapper : public exec {

    exec_wrapper(FunctionType&& function)
    : function_(function) {
    }

    void execute() override {
        function_();
    }

    FunctionType function_;
};

template <typename FunctionType>
auto make_exec(FunctionType&& function) {
    using ReturnType = ptr_concrete<exec_wrapper<FunctionType>>;
    return ReturnType(new exec_wrapper<FunctionType>(std::forward<FunctionType>(function)));
}

} // namespace common
} // namespace robot {namespace message
}