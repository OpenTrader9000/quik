
#pragma once

#include "config.hpp"
#include <memory>
#include <thread/telegram/telegram.hpp>

namespace robot {
namespace worker {

struct dispatcher {

    static void init(config const& conf);
    static void clear();
    static void stop();
    //
    // on_transaction
    // send_transaction
    // on_order

    static bool run();

    ~dispatcher();

    private:
    dispatcher();

    void init_internal(config const& conf);

    bool run_;

    using telegram_ptr_t = std::shared_ptr<thread::telegram::telegram>;

    telegram_ptr_t bot_thread_;

    static std::unique_ptr<dispatcher> instance_;
};
} // namespace worker
} // namespace robot