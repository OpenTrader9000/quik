#pragma once

#include "exec.hpp"
#include <common/numbers/bcd.hpp>
#include <string>

// transaction executes in lua thread
namespace robot {
namespace message {
namespace lua {

// values from quik
enum class action {
    NEW_ORDER,               // новая заявка,
    NEW_NEG_DEAL,            // новая заявка на внебиржевую сделку,
    NEW_QUOTE,               // новая безадресная заявка,
    KILL_QUOTE,              // снять безадресную заявку,
    MOVE_ORDERS,             // переставить заявки на рынке FORTS,
    NEW_REPO_NEG_DEAL,       // новая заявка на сделку РЕПО,
    NEW_EXT_REPO_NEG_DEAL,   // новая заявка на сделку модифицированного РЕПО(РЕПО - М),
    NEW_STOP_ORDER,          // новая стоп - заявка,
    KILL_ORDER,              // снять заявку,
    KILL_NEG_DEAL,           // снять заявку на внебиржевую сделку или заявку на сделку РЕПО,
    KILL_STOP_ORDER,         // снять стоп - заявку,
    KILL_ALL_ORDERS,         // снять все заявки из торговой системы,
    KILL_ALL_STOP_ORDERS,    // снять все стоп - заявки,
    KILL_ALL_NEG_DEALS,      // снять все заявки на внебиржевые сделки и заявки на сделки РЕПО,
    KILL_ALL_FUTURES_ORDERS, // снять все заявки на рынке FORTS,
    KILL_RTS_T4_LONG_LIMIT,  // удалить лимит открытых позиций на спот - рынке RTS Standard,
    KILL_RTS_T4_SHORT_LIMIT, // удалить лимит открытых позиций клиента по спот - активу на рынке
                             // RTS Standard,
    NEW_REPORT,   // новая  заявка - отчет о подтверждении транзакций в режимах РПС и РЕПО,
    SET_FUT_LIMIT // новое ограничение по фьючерсному счету

};

enum class operation { SELL, BUY };


struct send_transaction final : public exec {

    send_transaction();
    ~send_transaction();

    virtual void execute(lua_State* L) override;

    // /sell or /buy [l(everage)] code
    // choose portfolio. get portfolio - portfolio
    // get shares list if share doesn't binds (choose instrument)
    // choose price/choose


    std::string class_code_; // ids mby
    std::string sec_code_;   // ids mby
    unibcd_t price_;
    std::string account_;
    std::string client_code_;
    operation operation_;
    int64_t quantity_; //
    action action_;
};
} // namespace lua
} // namespace message
} // namespace robot