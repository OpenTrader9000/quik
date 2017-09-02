#include "run.hpp"
#include "dispatcher.hpp"

namespace robot {
namespace worker {
bool run() {
    return dispatcher::run();
}
} // namespace worker
} // namespace robot