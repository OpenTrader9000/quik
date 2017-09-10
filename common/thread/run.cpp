
#include "run.hpp"

namespace common {
namespace thread {

static bool const* run_flag = nullptr;
// flag for run a thread
void setup_run_flag(bool const& flag) {
    run_flag = &flag;
}

// return run flag
bool run() {
    return (run_flag == nullptr ? false : *run_flag);
}
} // namespace thread
} // namespace common