#pragma once

#include <persistent/config.hpp>
#include <memory>
#include "sql.hpp"

#include <persistent/trade/trade/cache.hpp>

namespace persistent {
namespace impl {



struct persistent {

    using trades_cache_t = ::persistent::trade::trade::cache;

    persistent(::persistent::config const& config);
    ~persistent();

    //void init(::persistent::config const& config);

    void start();
    
    /*
        STORAGE LOGIC
    */
    void push_mv(ptr_t&& message);
    void push_cp(ptr_t const& message);

    // THINK: Maybe flush signal or push everything


    /*
        TODO: PLAYER LOGIC
    */


    // handle signals
    // this signal must be placed after every possible sql or persistent signal
    void stop();
//private:

    /*
        SQL INTERFACE/DATA
    */

    sql            sql_;
    trades_cache_t trades_cache_;

    static std::unique_ptr<persistent> instance_;

};

} // namespace impl
} // namespace persistent