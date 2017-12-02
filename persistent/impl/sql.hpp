#pragma once

#include <common/message/message.hpp>

#include <common/thread/sink_with_logging.hpp>
#include <common/storage/scenario_entry.hpp>
#include <common/container/array_view.hpp>

namespace sqlpp {
namespace sqlite3 {
class connection;
}
} // namespace sqlpp


namespace persistent {
namespace impl {

using ptr_t = common::message::ptr;
using scenario_entry_t = common::storage::scenario_entry;


//using sqlite_conn_t      = sqlpp::sqlite3::connection;
//using sqlite_conn_conf_t = sqlpp::sqlite3::connection_config;
//using sink_mt_t          = common::thread::sink_mt_t;

// sql must be accessible from a multiple threads
struct sql : public common::thread::sink_with_logging_mt_t {

    sql(std::string const& path2db);
    ~sql();

    void connect();

    //void push(ptr_t&& src);

    //template <typename It>
    //void push(It iterator, size_t count);

    void         consume(ptr_t&& message);
    virtual void consume(std::vector<ptr_t>& messages) override;

    // function create new scenario id
    // \return scenarion_id in database
    int create_scenario(std::string const& name);

    // \brief extract entries by index and scenario name
    // \param scenario_name - name of existed scenario
    // \param idx - start index in table
    // \param count - max count of elements
    // \param callbacks - list of functions for an extraction
    // \return array of values
    std::vector<scenario_entry_t> extract_scenario_entries(std::string const& scenario_name,
                                                           int                idx,
                                                           unsigned           count,
                                                           std::vector<std::string> const& callbacks,
                                                           uint64_t start_date_in_ms,
                                                           uint64_t end_date_in_ms);

    // \brief create entry in session table
    // \param name - some session name
    // \return session_id
    int start_session(std::string name);


 private:
    void flush();

    void flush_unhandled();
    void flush_scenario();

    void create_database();

    std::string path2db_;

    // TODO: Think about of moving cache in other thread and about receiving bulk message
    std::vector<ptr_t> cache_;

    std::shared_ptr<sqlpp::sqlite3::connection>  connection_;
};

//template <typename It>
//inline void sql::push(It iterator, size_t count) {
//    sink_mt::push(it, count);
//}

} // namespace impl
} // namespace persistent
