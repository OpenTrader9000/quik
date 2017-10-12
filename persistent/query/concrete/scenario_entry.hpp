#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace query {
namespace concrete {
struct scenario_entry {
    std::string name;
    int64_t id     = 0; // start id
    unsigned limit = 0;
    std::vector<std::string> callbacks;

    template<typename ...Setters>
    scenario_entry(Setters&& ... setters) {
        using swallow = int[];
        (void)swallow {
            (std::forward<Setters>(setters)(*this), 0)...
        };
    }
};
} // namespace concrete
} // namespace query