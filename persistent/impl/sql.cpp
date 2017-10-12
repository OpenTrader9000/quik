#include "sql.hpp"

#include <persistent/ddl/generated/unhandled.h>
#include <sqlpp11/sqlite3/connection.h>
#include <sqlpp11/sqlite3/connection_config.h>
#include <unordered_set>

#include <utils/filesystem/filesystem.hpp>
#include <utils/string/string.hpp>

#include <common/container/container.hpp>

#include <persistent/ddl/generated/source.hpp>

namespace persistent {
namespace impl {

using namespace common::message;


sql::sql(std::string const& path2db)
: path2db_(path2db) {
    connect();
}

sql::~sql() {
    wait_sink();
    // write last data
    flush();
}

void sql::connect() {
    sqlpp::sqlite3::connection_config config;
    config.debug            = false;
    config.flags            = SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_CREATE;
    config.path_to_database = path2db_;

    bool file_exists = utils::fs::exists(path2db_);

    connection_ = std::make_shared<sqlpp::sqlite3::connection>(config);

    if (!file_exists)
        create_database();

}

void sql::create_database() {

    try {
        for (auto& script : gentab::queries) {
            auto queries = utils::split(script, ';');
            for (auto& query : queries) {
                connection_->execute(query);
            }
        }
    } catch (sqlpp::exception& ex) {
        //TODO: Logging
        (void)ex;
        
        
        // everithing is failed
        connection_.reset();

        utils::fs::remove_file(path2db_);
    }
}


//void sql::push(ptr_t&& src) {
//    sink_mt_t::push(std::move(src));
//}


void sql::flush() {

    // stop flush for a while
    if (!connection_) {
        connect();
    }

    common::container::stack_set<codes> code_set;

    // figure out what type of message is present now
    for (auto& p : cache_)
        code_set.insert(static_cast<codes>(p->code_));

    if (code_set.find(codes::UNHANDLED) != code_set.end())
        flush_unhandled();

    if (code_set.find(codes::SCENARIO_ENTRY) != code_set.end())
        flush_scenario();


    cache_.clear();
 }

void sql::consume(ptr_t&& message) {


    assert(is_sql(message->code_) || is_flush(message->code_));

    if (is_sql(message->code_)) {
        cache_.push_back(std::move(message));
    } else if (is_flush(message->code_)) {

        // TODO: log/error log
        try {
            flush();
        } catch (...) {
            // close connection
            connection_.reset();
        }
    }
}


} // namespace impl
} // namespace persistent
