#include "writer.hpp"
#include "reader.hpp"

#include <iomanip>
#include <unordered_set>
#include <time.h>

#include <utils/string/build.hpp>
#include <utils/filesystem/mapped_file.hpp>
#include <utils/filesystem/filesystem.hpp>

#include <common/storage/serialize.hpp>
#include <common/storage/compress/compress.hpp>
#include <common/message/trade/trade.hpp>
#include <common/numbers/cast.hpp>

#include <boost/container/flat_set.hpp>

#include <zlib/zlib.h>

namespace persistent {
namespace trade {
namespace trade {

static unsigned place_for_header = common::storage::place_for_data<header>();
static unsigned place_for_trade  = common::storage::place_for_data<common::storage::trade>();

writer::writer(std::string const& path, std::string const& sec_code)
    : header_{}
    , path_(path)
    , sec_code_(sec_code) {
    filestream_.exceptions(std::ios::failbit | std::ios::badbit);
}

writer::~writer() {}

bool writer::open(){

    bool existed = utils::fs::exists(path_);
    if (!existed) {

        utils::fs::mkdir_for_file(path_);

        // init header
        strncpy(header_.sec_code_, sec_code_.c_str(), sizeof(header::sec_code_));
        header_.version_ = 1;
    }
    // open a file
    filestream_.open(path_, std::ios_base::out | (existed ? std::ios_base::in : 0) | std::ios_base::binary);

    if (existed) {
        read_header();
    } else {
        update_header();
    }

    return true;
}

bool writer::is_file_compressed() {
    return (header_.flags_ & (1 << header::COMPRESSED)) != 0;
}

void writer::filter(trades_storage_t & trades){
    boost::container::flat_set<uint64_t> trade_numbers;
    trades_storage_t    filtered_sequence;
    filtered_sequence.reserve(trades.size());

    std::copy_if(trades.begin(), trades.end(), std::back_inserter(filtered_sequence), [&](common::storage::trade& t) mutable {
        // check if element allready present
        if (trade_numbers.find(t.trade_num_) != trade_numbers.end()) {
            return false;
        }
        // add non presented element
        trade_numbers.insert(t.trade_num_);
        return true;
    });
    std::swap(trades, filtered_sequence);
}

bool writer::compress() {

    utils::fs::mapped_file mapped_file(path_.c_str());

    // header need for a new file
    auto header_pointer = reinterpret_cast<unsigned char*>(mapped_file.data());
    common::storage::visit_fields(header_, [&, pointer = header_pointer](char* buffer, unsigned size) mutable {
        std::copy(pointer, pointer + size, buffer);
        pointer += size;
    });

    if (is_file_compressed())
        return true;

    // **********************
    //   READING QUOTES
    // **********************
    std::vector<common::storage::trade> deserialized_trades;

    auto data_pointer = header_pointer + place_for_header;
    auto file_size = mapped_file.length();
    auto end_pointer = header_pointer + file_size;

    assert(place_for_trade * header_.trades_count_ == end_pointer - data_pointer);

    auto pointer = data_pointer;
    while (pointer != end_pointer) {
        common::storage::trade trade;

        // read buffer and check every field
        common::storage::visit_fields(trade, [&](char* c, unsigned size) {

            if (pointer + size > end_pointer) {
                throw std::runtime_error("Buffer overflow during quotes reading");
            }

            std::copy(pointer, pointer + size, c);
            pointer += size;

        });

        deserialized_trades.push_back(trade);
    }

    mapped_file.close();

    // figure out proper count of quotes
    assert(deserialized_trades.size() == header_.trades_count_);

    filter(deserialized_trades);

    unsigned deserialized_quotes_size = common::numbers::integer_cast<unsigned>(deserialized_trades.size() * place_for_trade);

    // ********************

    // ********************
    //   COMPRESSION
    // ********************

    std::vector<unsigned char> compressed_data;
    compressed_data.reserve(deserialized_quotes_size);
    auto compressor = common::storage::trade::make_compressor();
    for (auto& value : deserialized_trades) {
        common::storage::compress::encode(value, compressor, [&](common::container::array_view<unsigned char> view) {
            std::copy(view.begin(), view.end(), std::back_inserter(compressed_data));
        });
    }

    unsigned original_size = static_cast<unsigned>(compressed_data.size());
    uLongf compressed_size = original_size;

    std::vector<unsigned char> zipped_data;
    zipped_data.resize(deserialized_quotes_size);
    compress2(zipped_data.data(), &compressed_size, compressed_data.data(), common::numbers::integer_cast<uLong>(compressed_data.size()), 9);
    zipped_data.resize(compressed_size);
    // ********************

    // writing a new file
    std::string   new_path = path_ + "_tmp";
    std::ofstream compressed_file(new_path, std::ios_base::binary | std::ios_base::out);

    auto zipped_file_header = header_;
    zipped_file_header.flags_ |= (1 << header::COMPRESSED);
    zipped_file_header.trades_count_ = deserialized_trades.size();

    common::storage::visit_fields(zipped_file_header, [&](char* buffer, unsigned size) {
        compressed_file.write(buffer, size);
    });

    compressed_file.write((char*)&original_size, sizeof(original_size));
    compressed_file.write((char*)zipped_data.data(), zipped_data.size());
    compressed_file.close();

    // *********************
    //      CHECKING
    // ********************

    std::vector<common::storage::trade> unzipped_trades;

    {
        // map and unmap file
        reader r(new_path);
        r.read_all_trades(unzipped_trades);
    }

    if (unzipped_trades.size() != deserialized_trades.size()) {
        throw std::runtime_error("Unable to compress quotes. Sizes doesn't match.");
    }

    for (unsigned ix = 0; ix < unzipped_trades.size(); ++ix) {

        auto& before = deserialized_trades[ix];
        auto& after = unzipped_trades[ix];

        utils::apply_tuple(
            [&](auto field) {
            if (before.*field != after.*field) {
                throw std::runtime_error("Field's values doesn't match.");
            }
        },
        common::storage::trade::fields());
    }

    // ********************

    utils::fs::remove_file(path_);
    // move compressed file on it's place
    utils::fs::move(new_path, path_);

    return true;
}

void writer::read_header() {
	filestream_.seekg(0);
	common::storage::visit_fields(header_, [&](char* buffer, unsigned size) {
		filestream_.read(buffer, size);
	});
}

void writer::update_header()
{
	filestream_.seekp(0);
	common::storage::visit_fields(header_, [&](char* buffer, unsigned size) {
		filestream_.write(buffer, size);
	});

}

void writer::serialize(messages_view_t messages) {

    using trade_t = common::message::trade::trade;

    //std::vector<common::storage::trade> trades;
    //trades.reserve(count);
    
    // File contains many valuable data
    open();

    // no throw no anything - just exit
    if (is_file_compressed())
        return;

    std::vector<char> trades_buffer;
    trades_buffer.reserve(place_for_trade*messages.size());

    for (auto& p : messages) {
        // trade exist, trade has proper time
        assert(p);

        // trade_holder will destroied at the end of the scope
        auto trade_holder = p.cast<trade_t>();
        
        common::storage::visit_fields(trade_holder->trade_, [&](char* buffer, unsigned size) {
            std::copy(buffer, buffer + size, std::back_inserter(trades_buffer));
        });
        ++header_.trades_count_;
    }

    // no one trade is add
    if (trades_buffer.empty())
        return;

    update_header();

    filestream_.seekp(0, std::fstream::end);
    filestream_.write(trades_buffer.data(), trades_buffer.size());
    filestream_.flush();

}



}
} // namespace trade
} // namespace persistent

