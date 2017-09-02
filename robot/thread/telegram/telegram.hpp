
#pragma once

#include <string>
#include <thread>
//#include <tgbot/tgbot.h>
#include <memory>
#include <vector>

namespace TgBot {
class Bot;
class Message;
} // namespace TgBot

namespace robot {
namespace thread {
namespace telegram {

using tg_mes_ptr_t = std::shared_ptr<TgBot::Message>;

struct telegram {


    telegram(std::string token, std::vector<int64_t> ids);
    ~telegram();

    void on_command_portfolio(tg_mes_ptr_t);
    void on_command_order(tg_mes_ptr_t);
    void on_command_limits(tg_mes_ptr_t);

    void message_state_mashine(tg_mes_ptr_t mes);

    void run_thread();

    TgBot::Bot* bot_;
    std::vector<int64_t> allowed_ids_;

    std::thread thread_;
};
} // namespace telegram
} // namespace thread
} // namespace robot