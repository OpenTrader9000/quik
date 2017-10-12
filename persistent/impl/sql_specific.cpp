#include "sql.hpp"

#include <sqlpp11/connection.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/join.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/without_table_check.h>
#include <common/container/array_view.hpp>
#include <sqlpp11/sqlite3/connection.h>
// concrete tables
#include <persistent/ddl/generated/scenario.h>
#include <persistent/ddl/generated/session.h>

// file with queries that has a specific logic (not strongly insert, remove, update or select)

namespace persistent {
namespace impl {

template <typename TableType>
int create_with_id(std::shared_ptr<sqlpp::sqlite3::connection> conn, std::string const& name) {

    assert(conn);
    auto& db = *conn;

    TableType tab;

    // create
    // return id
    db(sqlpp::insert_into(tab).set(tab.name = name));
    auto result = db(sqlpp::select(sqlpp::max(tab.id)).from(tab).unconditionally()).front().max;
    return static_cast<int>(result);
}

int sql::create_scenario(std::string const& name) {
    return create_with_id<gentab::Scenario>(connection_,name);;
}

std::vector<scenario_entry_t> sql::extract_scenario_entries(std::string const& scenario_name,
                                                            int idx,
                                                            unsigned count,
                                                            common::container::array_view<std::string> callbacks) {
    std::vector<scenario_entry_t> result;
    
    gentab::Scenario scenario_tab{};
    gentab::ScenarioEntry scenario_entry_tab{};

    assert(connection_);

    auto saved_connection = connection_;

    auto& db = *connection_;

    // make query by scenario name
    auto query_result =
    db(sqlpp::select(sqlpp::all_of(scenario_entry_tab))
       .from(scenario_entry_tab.join(scenario_tab)
             .on(scenario_entry_tab.scenarioId == scenario_tab.id and scenario_tab.name == scenario_name))
       .where(scenario_entry_tab.id > idx and scenario_entry_tab.name.in(sqlpp::value_list(callbacks) ))
       .limit(count));

    // fill result
    for (auto& row : query_result) {
        result.emplace_back();
        scenario_entry_t& element = result.back();

        element.body_ = row.body;
        element.idx_ = row.id;
        element.info_ = row.info;
        element.name_ = row.name;
        element.scenario_id_ = row.scenarioId;
        element.timestamp_ = row.ts;
        element.type_ = row.type;
    }

    return result;
}

int sql::start_session(std::string name) {
    return create_with_id<gentab::Session>(connection_, name);;
}

} // namespace impl
} // namespace persistent