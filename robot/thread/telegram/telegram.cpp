#include "telegram.hpp"
#include <message/lua/exec.hpp>
#include <tgbot/tgbot.h>
#include <thread/lua/lua.hpp>
#include <worker/run.hpp>

namespace robot {
namespace thread {
namespace telegram {

telegram::telegram(std::string token, std::vector<int64_t> ids)
: bot_(nullptr)
, allowed_ids_(ids) {

    bot_ = new TgBot::Bot(token);
    bot_->getEvents().onCommand("portfolio",
                                [this](tg_mes_ptr_t mes) { on_command_portfolio(std::move(mes)); });
    bot_->getEvents().onCommand("order",
                                [this](tg_mes_ptr_t mes) { on_command_order(std::move(mes)); });
    bot_->getEvents().onCommand("limits",
                                [this](tg_mes_ptr_t mes) { on_command_limits(std::move(mes)); });
    bot_->getEvents().onAnyMessage(
    [this](tg_mes_ptr_t mes) { message_state_mashine(std::move(mes)); });

    run_thread();
}

telegram::~telegram() {
    if (thread_.joinable()) {
        thread_.join();
    }

    if (bot_ != nullptr)
        delete bot_;
}

void telegram::run_thread() {

    if (thread_.joinable())
        thread_.join();

    thread_ = std::thread([this]() {
        try {
            // printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
            TgBot::TgLongPoll longPoll(*bot_);
            while (worker::run())
                longPoll.start();
        } catch (TgBot::TgException&) {
            if (!worker::run())
                return;

            // run this routine in lua thread
            lua::enqueue_task(message::lua::make_exec([this](lua_State*) { run_thread(); }));
        }
    });
}

void telegram::on_command_portfolio(tg_mes_ptr_t mes) {
}

void telegram::on_command_order(tg_mes_ptr_t mes) {
}
void telegram::message_state_mashine(tg_mes_ptr_t mes) {
}
void telegram::on_command_limits(tg_mes_ptr_t mes) {
}
} // namespace telegram
} // namespace thread
} // namespace robot