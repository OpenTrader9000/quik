#include "datasource.hpp"
#include "persistent_datasource.hpp"

namespace view {
namespace model {

datasource::datasource(unsigned flags)
    : flags_(flags) {}

datasource::~datasource() {}

unsigned datasource::flags() const {
    return flags_;
}

bool datasource::has_period(period_t period) const {
    switch (period) {
    case persistent::trade::period::p1min:
        return (flags_ & p1min) != 0;
    case persistent::trade::period::p5min:
        return (flags_ & p5min) != 0;
    case persistent::trade::period::p10min:
        return (flags_ & p10min) != 0;
    case persistent::trade::period::p15min:
        return (flags_ & p15min) != 0;
    case persistent::trade::period::p30min:
        return (flags_ & p30min) != 0;
    case persistent::trade::period::p60min:
        return (flags_ & phour) != 0;
    case persistent::trade::period::pday:
        return (flags_ & pday) != 0;
    case persistent::trade::period::pweek:
        return (flags_ & pweek) != 0;
    }
    return false;
}

datasource_ptr_t g_source;
void make_persistent_datasource(std::string const& path2archive) {
    g_source = std::make_shared<persistent_datasource>(path2archive);
}

datasource_ptr_t get_datasource(){
    return g_source;
}

} // namespace model
} // namespace view
