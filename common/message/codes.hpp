#pragma once

namespace common {
namespace message {

enum code_type {
    SQL_CODE    = 0x10000,
    TRADE_CODES = 0x20000,
};

enum class codes {
    EXEC_LUA = 1,
    EXEC,
    FLUSH,
    // ON_TRADE,
    // ON_TRADES,
    // ON_ORDER,
    // ON_TRANSACTION,
    // SEND_TRANSACTION,
    // ON_ACCOUNT_BALANCE,
    // ON_ACCOUNT_POSITION,
    // ON_ALL_TRADE,
    // ON_DEPO_LIMIT,
    // ON_DEPO_LIMIT_DELETE,
    // ON_FUTURES_CLIENT_HOLDING,
    // ON_FUTURES_LIMIT_CHANGE,
    // ON_FUTURES_LIMIT_DELETE,
    // ON_MONEY_LIMIT,
    // ON_MONEY_LIMIT_DELETE,
    // ON_NEG_DEAL,
    // ON_NEG_TRADE,
    // ON_STOP,
    // ON_STOP_ORDER,
    // ON_TRANS_REPLY,
    STOP,
    START,
    COMPRESS,
    UNHANDLED = SQL_CODE,
    SCENARIO_ENTRY,
    EVENT,
    TRADE = TRADE_CODES,
    QUOTE,
    QUOTES
};

inline bool is_sql(int code) {
    return (code & static_cast<int>(code_type::SQL_CODE)) != 0;
}

inline bool is_flush(int code) {
    return (code & static_cast<int>(codes::FLUSH)) != 0;
}
} // namespace message
} // namespace common