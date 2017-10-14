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
serializer::serializer(std::string const& sec_code, std::string const& storage_folder, uint64_t start_time, uint64_t end_time)
: file(sec_code, storage_folder, start_time / 1000 + 1, mode::WRITE)
, start_time_(start_time)
, end_time_(end_time) {}

serializer::~serializer() {} 

void serializer::serialize(messages_view_t messages, unsigned count) {

    using trade_t = common::message::trade::trade;

    //std::vector<common::storage::trade> trades;
    //trades.reserve(count);

	std::vector<char> trades_buffer;
	trades_buffer.reserve(sizeof(common::storage::trade));

	// File contains many valuable data
	open();

    unsigned trades_count  = 0;
    for (auto& p : messages) {
        // trade exist, trade has proper time
        if (!p)
            continue;
        
		// fiter possible external trades
        auto ptr = p.cast<trade_t>();
        auto srv_time = ptr->server_timestamp();
        if (ptr->sec_code_ != sec_code_ || srv_time < start_time_ || srv_time >= end_time_) {
            continue;
        }

        // trade_holder will destroied at the end of the scope
        auto trade_holder = p.extract<trade_t>();

		++trades_count;

		auto& trade = trade_holder->trade_;
		common::storage::visit_fields(trade, [&](char* buffer, unsigned size) {
			std::copy(buffer, buffer + size, std::back_inserter(trades_buffer));
		});
    }
	
	// no one trade is add
    if (trades_buffer.empty())
        return;

    filestream_.seekp(0, std::fstream::end);
    filestream_.write(trades_buffer.data(), trades_buffer.size());

	filestream_.flush();

    header_.trades_count_ += trades_count;
	 
	update_header();
}

} // namespace trade
} // namespace trade
} // namespace persistent