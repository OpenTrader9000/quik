#include "send_transaction.hpp"


namespace robot {
namespace message {
namespace lua {

char const* to_string(action act) {
#define ACTION(NAME)   \
    case action::NAME: \
        return #NAME;

    switch (act) {
        ACTION(NEW_ORDER);
        ACTION(NEW_NEG_DEAL);
        ACTION(NEW_QUOTE);
        ACTION(KILL_QUOTE);
        ACTION(MOVE_ORDERS);
        ACTION(NEW_REPO_NEG_DEAL);
        ACTION(NEW_EXT_REPO_NEG_DEAL);
        ACTION(NEW_STOP_ORDER);
        ACTION(KILL_ORDER);
        ACTION(KILL_NEG_DEAL);
        ACTION(KILL_STOP_ORDER);
        ACTION(KILL_ALL_ORDERS);
        ACTION(KILL_ALL_STOP_ORDERS);
        ACTION(KILL_ALL_NEG_DEALS);
        ACTION(KILL_ALL_FUTURES_ORDERS);
        ACTION(KILL_RTS_T4_LONG_LIMIT);
        ACTION(KILL_RTS_T4_SHORT_LIMIT);
        ACTION(NEW_REPORT);
        ACTION(SET_FUT_LIMIT);
    default:
        assert(false);
    };
#undef ACTION
    return "";
}

char const* to_string(operation op) {
    switch (op) {
    case operation::BUY:
        return "B";
    case operation::SELL:
        return "S";
    default:
        assert(false);
    }
    return "";
}

send_transaction::send_transaction() {
}
send_transaction::~send_transaction() {
}

void send_transaction::execute(lua_State* L) {
}


} // namespace lua
} // namespace message
} // namespace robot