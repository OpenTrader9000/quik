#pragma once

#include "sql.hpp"
#include <sqlpp11/connection.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlite3/connection.h>


// concrete tables
#include <persistent/ddl/generated/unhandled.h>
#include <persistent/ddl/generated/scenario.h>

// concrete messages
#include <common/message/general/unhandled.hpp>
#include <common/message/scenario/entry.hpp>

namespace persistent {
namespace impl {

using namespace sqlpp;

void sql::flush_unhandled() {

    //
    auto& db = *connection_;

    gentab::Unhandled unhandledTable;

    auto tx = start_transaction(db);

    auto prep = db.prepare(insert_into(unhandledTable)
                           .set(unhandledTable.ts   = parameter(unhandledTable.ts),
                                unhandledTable.name = parameter(unhandledTable.name),
                                unhandledTable.body = parameter(unhandledTable.body),
                                unhandledTable.type = parameter(unhandledTable.type)));

    for (auto& p : cache_) {

        if (!p || p->code_ != static_cast<int>(common::message::codes::UNHANDLED))
            continue;

        auto concrete = p.extract<common::message::general::unhandled>();

        prep.params.ts   = concrete->timestamp();
        prep.params.body = concrete->body();
        prep.params.name = concrete->name();
        prep.params.type = concrete->body_.type_;

        db(prep);
    }

    tx.commit();
}


void sql::flush_scenario()
{
    auto& db = *connection_;

    gentab::ScenarioEntry scenarioEntry;

    auto tx = start_transaction(db);

    auto prep = db.prepare(insert_into(scenarioEntry)
        .set(scenarioEntry.ts = parameter(scenarioEntry.ts),
            scenarioEntry.name = parameter(scenarioEntry.name),
            scenarioEntry.body = parameter(scenarioEntry.body),
            scenarioEntry.type = parameter(scenarioEntry.type),
            scenarioEntry.info = parameter(scenarioEntry.info),
            scenarioEntry.scenarioId =parameter(scenarioEntry.scenarioId)));

    for (auto& p : cache_) {

        if (!p || p->code_ != static_cast<int>(common::message::codes::SCENARIO_ENTRY))
            continue;

        auto concrete = p.extract<common::message::scenario::entry>();

        prep.params.ts         = concrete->timestamp();
        prep.params.body       = concrete->body();
        prep.params.name       = concrete->name();
        prep.params.type       = concrete->body_.type_;
        prep.params.scenarioId = concrete->scenario_id();
        prep.params.info       = concrete->info();

        db(prep);
    }
    tx.commit();
}
} // namespace impl
} // namespace persistent