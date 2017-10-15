#include "serializer.hpp"
#include <common/storage/trade.hpp>
#include <common/storage/serialize.hpp>
#include <common/message/trade/quote.hpp>
#include <common/message/trade/quotes.hpp>
#include <utils/filesystem/filesystem.hpp>
#include <utils/string/build.hpp>


#include <time.h>

#include <iomanip>

#include <iostream>
#include <algorithm>

namespace persistent {
namespace trade {
namespace quote {

serializer::serializer(std::string const& sec_code, std::string const& storage_folder, uint64_t start_time)
: file(sec_code, storage_folder, start_time / 1000 + 1, mode::WRITE)
, start_time_(start_time) {}

serializer::~serializer() {} 

void serializer::serialize(messages_view_t messages, unsigned count) {

    using quote_t = common::message::trade::quote;
    using quotes_t = common::message::trade::quotes;

    //std::vector<common::storage::trade> trades;
    //trades.reserve(count);

	std::vector<char> trades_buffer;
	trades_buffer.reserve(sizeof(common::storage::trade));

	// File contains many valuable data
	open();

    size_t quotes_count = 0;
    for (auto& p : messages) {
        // trade exist, trade has proper time
        if (!p)
            continue;

		auto* view = p.cast<quotes_t>();
		if (view->sec_code_ != sec_code_)
			continue;

        // trade_holder will destroied at the end of the scope
        auto quote_holder = p.extract<quotes_t>();

		quotes_count += quote_holder->quotes_.size();

        for (auto& quote : quote_holder->quotes_) {
            common::storage::visit_fields(quote, [&](char* buffer, unsigned size) {
                std::copy(buffer, buffer + size, std::back_inserter(trades_buffer));
            });
        }
    }
	
	// no one trade is add
    if (trades_buffer.empty())
        return;

    filestream_.seekp(0, std::fstream::end);
    filestream_.write(trades_buffer.data(), trades_buffer.size());

	filestream_.flush();

    header_.quotes_count_ += quotes_count;
	 
	update_header();
}

} // namespace trade
} // namespace trade
} // namespace persistent