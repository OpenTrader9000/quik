#include "scenario.hpp"


namespace common {
namespace scenario {

int s_id = 0;
int id() {
    return s_id;
}

void set(int v) {
    s_id = v;
}

} // namespace scenario
} // namespace common