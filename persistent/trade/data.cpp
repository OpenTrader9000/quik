#pragma once
#include "data.hpp"


namespace persistent {
namespace trade {

uint64_t period_in_ms(period p) {
    // choose time
    switch (p) {
    case period::p1min:
        return 60 * 1000;
    case period::p5min:
        return 300 * 1000;
    case period::p10min:
        return 600 * 1000;
    case period::p15min:
        return 900 * 1000;
    case period::p30min:
        return 1800 * 1000;
    case period::p60min:
        return 3600 * 1000;
    case period::pday:
        return 24 * 3600 * 1000;
    case period::pweek:
        return 7 * 24 * 7 * 1000;
    }
    return 0;
}

uint64_t series::start() const {
    if (series_.empty())
        return 0;
    return series_.front().open_timestamp_;
}

uint64_t series::end() const {
    if (series_.empty())
        return 0;
    return series_.back().open_timestamp_ + period_in_ms(period_);
}


} // namespace trade
} // namespace persistent