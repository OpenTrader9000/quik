#pragma once
#include <string>
#include <vector>


namespace robot {
namespace worker {
struct telegram_config {
    std::string bot_token_;
    std::vector<int64_t> ids_;
};

struct config {
    telegram_config telegram_;
};
} // namespace worker
} // namespace robot