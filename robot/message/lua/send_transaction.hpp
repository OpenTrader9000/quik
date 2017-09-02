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
    NEW_ORDER,               // ����� ������,
    NEW_NEG_DEAL,            // ����� ������ �� ����������� ������,
    NEW_QUOTE,               // ����� ����������� ������,
    KILL_QUOTE,              // ����� ����������� ������,
    MOVE_ORDERS,             // ����������� ������ �� ����� FORTS,
    NEW_REPO_NEG_DEAL,       // ����� ������ �� ������ ����,
    NEW_EXT_REPO_NEG_DEAL,   // ����� ������ �� ������ ����������������� ����(���� - �),
    NEW_STOP_ORDER,          // ����� ���� - ������,
    KILL_ORDER,              // ����� ������,
    KILL_NEG_DEAL,           // ����� ������ �� ����������� ������ ��� ������ �� ������ ����,
    KILL_STOP_ORDER,         // ����� ���� - ������,
    KILL_ALL_ORDERS,         // ����� ��� ������ �� �������� �������,
    KILL_ALL_STOP_ORDERS,    // ����� ��� ���� - ������,
    KILL_ALL_NEG_DEALS,      // ����� ��� ������ �� ����������� ������ � ������ �� ������ ����,
    KILL_ALL_FUTURES_ORDERS, // ����� ��� ������ �� ����� FORTS,
    KILL_RTS_T4_LONG_LIMIT,  // ������� ����� �������� ������� �� ���� - ����� RTS Standard,
    KILL_RTS_T4_SHORT_LIMIT, // ������� ����� �������� ������� ������� �� ���� - ������ �� �����
                             // RTS Standard,
    NEW_REPORT,   // �����  ������ - ����� � ������������� ���������� � ������� ��� � ����,
    SET_FUT_LIMIT // ����� ����������� �� ����������� �����

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