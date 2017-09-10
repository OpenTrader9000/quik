#pragma once
#include <string>
#include <vector>
#include <persistent/config.hpp>


namespace robot {
namespace worker {
struct telegram_config {
    std::string bot_token_;
    std::vector<int64_t> ids_;

    explicit operator bool() const {
        return !bot_token_.empty();
    }
};

struct config {
    telegram_config telegram_;
    ::persistent::config persistent_;
};
} // namespace worker
} // namespace robot