#include "serializer.hpp"
#include <common/storage/trade.hpp>
#include <common/storage/serialize.hpp>
#include <common/message/trade/trade.hpp>
#include <utils/filesystem/filesystem.hpp>
#include <utils/string/build.hpp>


#include <time.h>

#include <iomanip>

#include <iostream>
#include <algorithm>

namespace persistent {
namespace trade {
namespace trade {
serializer::serializer(std::string const& sec_code, std::string const& storage_folder)
: file(sec_code, storage_folder, mode::WRITE)
 {}

serializer::~serializer() {} 

void serializer::serialize(messages_view_t messages, unsigned count, uint64_t start_time, uint64_t end_time) {

    using trade_t = common::message::trade::trade;

    //std::vector<common::storage::trade> trades;
    //trades.reserve(count);

	std::vector<char> trades_buffer;
	trades_buffer.reserve(sizeof(common::storage::trade));

	// File contains many valuable data
	open(start_time / 1000 + 1);

    unsigned trades_count  = 0;
    uint64_t max_timestamp = 0;
    for (auto& p : messages) {
        // trade exist, trade has proper time
        if (!p)
            continue;
        
        auto ptr = p.cast<trade_t>();
        auto srv_time = ptr->server_timestamp();
        if (ptr->sec_code_ != sec_code_ || (srv_time < start_time && srv_time >= end_time)) {
            continue;
        }

        // all criterias are passed. But trade might be stay from a previous day
        if (srv_time < header_.last_timestamp_in_ms_) {
            std::cerr << "removed trade, srv_time: " << srv_time << ", last_timestamp: " << header_.last_timestamp_in_ms_ << "\n";
            p.reset();
            continue;
        }

        max_timestamp = std::max(max_timestamp, srv_time);
		++trades_count;

		// trade_holder will destroied at the end of the scope
        auto trade_holder = p.extract<trade_t>();
		auto& trade = trade_holder->trade_;
        common::storage::serialize(trades_buffer, trade.flags_, trade.price_, trade.machine_timestamp_,
                                   trade.server_timestamp_, trade.quantity_, trade.open_interest_);
    }
	
	// no one trade is add
    if (trades_buffer.empty())
        return;

    filestream_.seekp(0, std::fstream::end);
    filestream_.write(trades_buffer.data(), trades_buffer.size());
	filestream_.flush();

    header_.trades_count_ += trades_count;
    header_.last_timestamp_in_ms_ = max_timestamp;

	update_header();
    //update_trades_count();
    //update_timestamp();
	//
	//filestream_.seekp(0, std::fstream::end);
	//
	//
	////filestream_.flush();
	//filestream_.close();
    
}

} // namespace trade
} // namespace trade
} // namespace persistent