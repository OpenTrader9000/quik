#pragma once
#include <string>
#include <vector>
#include <persistent/config.hpp>
#include <utils/log/log.hpp>

namespace robot {
namespace worker {
struct telegram_config {
    std::string bot_token_;
    std::vector<int64_t> ids_;

    explicit operator bool() const {
        return !bot_token_.empty();
    }
};

struct log_config {
    utils::log::log_level level_;
    std::string           path_;

    void parse_level(std::string const& lev) {
        if (lev.compare("INFO") == 0) {
            level_ = utils::log::log_level::LOG_LEVEL_INFO;
        } else if (lev.compare("ERROR") == 0) {
            level_ = utils::log::log_level::LOG_LEVEL_ERROR;
        } else if (lev.compare("CRITICAL") == 0) {
            level_ = utils::log::log_level::LOG_LEVEL_CRITICAL;
        } else if (lev.compare("DEBUG") == 0) {
            level_ = utils::log::log_level::LOG_LEVEL_DEBUG;
        } else if (lev.compare("FATAL") == 0) {
            level_ = utils::log::log_level::LOG_LEVEL_FATAL;
        } else if (lev.compare("TRACE") == 0) {
            level_ = utils::log::log_level::LOG_LEVEL_TRACE;
        } else if (lev.compare("VERBOSE") == 0) {
            level_ = utils::log::log_level::LOG_LEVEL_VERBOSE;
        } else if (lev.compare("WARNING") == 0) {
            level_ = utils::log::log_level::LOG_LEVEL_WARNING;
        }
    }
};

struct config {
    telegram_config telegram_;
    ::persistent::config persistent_;
    log_config  log_;
};
} // namespace worker
} // namespace robot