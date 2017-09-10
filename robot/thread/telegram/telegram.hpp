
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

void init(std::string token, std::vector<int64_t> ids);
void stop();


} // namespace telegram
} // namespace thread
} // namespace robot