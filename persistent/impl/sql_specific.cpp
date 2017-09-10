#include "sql.hpp"

#include "sql.hpp"
#include <sqlpp11/connection.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/select.h>
#include <sqlpp11/sqlite3/connection.h>

// concrete tables
#include <persistent/ddl/generated/scenario.h>

// file with queries that has a specific logic (not strongly insert, remove, update or select)

namespace persistent {
namespace impl {
uint64_t sql::create_scenario(std::string const& name) {
    return 0;    
}
} // namespace impl
} // namespace persistent