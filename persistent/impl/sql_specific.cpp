#include "sql.hpp"

#include <sqlpp11/connection.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/sqlite3/connection.h>
// concrete tables
#include <persistent/ddl/generated/scenario.h>

// file with queries that has a specific logic (not strongly insert, remove, update or select)

namespace persistent {
namespace impl {
int sql::create_scenario(std::string const& name) {

    assert(connection_);
    auto& db = *connection_;

    gentab::Scenario scenario_tab;

    // delete
    // create
    // return id
    db(sqlpp::remove_from(scenario_tab).where(scenario_tab.name == name));
    db(sqlpp::insert_into(scenario_tab).set(scenario_tab.name = name));
    auto result = db(sqlpp::select(sqlpp::max(scenario_tab.id)).from(scenario_tab).unconditionally()).front().max;
    return static_cast<int>(result);;
    //return 0;
}
} // namespace impl
} // namespace persistent