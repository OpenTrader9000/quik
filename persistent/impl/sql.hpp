#pragma once

#include <common/message/message.hpp>

#include <common/thread/sink.hpp>

namespace sqlpp {
namespace sqlite3 {
class connection;
}
} // namespace sqlpp


namespace persistent {
namespace impl {

using ptr_t = common::message::ptr;

//using sqlite_conn_t      = sqlpp::sqlite3::connection;
//using sqlite_conn_conf_t = sqlpp::sqlite3::connection_config;
using sink_mt_t          = common::thread::sink_mt_t;

// sql must be accessible from a multiple threads
struct sql : public sink_mt_t {

    sql(std::string const& path2db);
    ~sql();

    void connect();

    void push(ptr_t&& src);

    template <typename It>
    void push(It iterator, size_t count);

    virtual void consume(ptr_t&& message) override;

    // function removes scenario with name and adds as new one
    // \return scenarion_id in database
    uint64_t create_scenario(std::string const& name);

 private:
    void flush();

    void flush_unhandled();

    void create_database();

    std::string path2db_;

    // TODO: Think about of moving cache in other thread and about receiving bulk message
    std::vector<ptr_t> cache_;

    std::shared_ptr<sqlpp::sqlite3::connection>  connection_;
};

template <typename It>
inline void sql::push(It iterator, size_t count) {
    sink_mt::push(it, count);
}

} // namespace impl
} // namespace persistent
