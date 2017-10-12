#include "session.hpp"
#include <assert.h>

namespace common {
namespace session {
int session_id = 0;
int id() {
    assert(session_id != 0);
    return session_id;
}
void set(int id) {
    assert(session_id == 0);
    session_id = id;
}
} // namespace session
} // namespace common