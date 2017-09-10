#pragma once

#include "sql.hpp"
#include <sqlpp11/connection.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlite3/connection.h>

// concrete tables
#include <persistent/ddl/generated/unhandled.h>

// concrete messages
#include <common/message/general/unhandled.hpp>

namespace persistent {
namespace impl {

using namespace sqlpp;

void sql::flush_unhandled() {

    //
    auto& db = *connection_;

    gentab::Unhandled unhandledTable;


    auto prep = db.prepare(insert_into(unhandledTable)
                           .set(unhandledTable.ts   = parameter(unhandledTable.ts),
                                unhandledTable.name = parameter(unhandledTable.name),
                                unhandledTable.body = parameter(unhandledTable.body),
                                unhandledTable.type = parameter(unhandledTable.type)));

    for (auto& p : cache_) {

        if (!p || p->code_ != static_cast<int>(common::message::codes::UNHANDLED))
            continue;

        auto concrete = p.cast<common::message::general::unhandled>();

        prep.params.ts   = concrete->timestamp();
        prep.params.body = concrete->body();
        prep.params.name = concrete->name();
        prep.params.type = concrete->body_.type_;

        db(prep);
    }
}
} // namespace impl
} // namespace persistent