#include "base.hpp"

namespace common {
namespace message {
base::base(int code)
: code_(code)
, counter_(0) {
}
base::~base() {
}


} // namespace message
} // namespace common