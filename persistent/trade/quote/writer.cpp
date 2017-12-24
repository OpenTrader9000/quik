#include "writer.hpp"

#include <utils/string/build.hpp>
#include <iomanip>
#include <unordered_set>
#include <time.h>

#include <utils/filesystem/filesystem.hpp>
#include <utils/filesystem/mapped_file.hpp>
#include <common/storage/serialize.hpp>
#include <common/message/trade/quote.hpp>
#include <common/message/trade/quotes.hpp>
#include <common/storage/compress/compress.hpp>

#include <zlib/zlib.h>

#include <iterator>

#include "reader.hpp"

namespace persistent {
namespace trade {
namespace quote {

static const unsigned place_for_quote  = common::storage::place_for_data<common::storage::quote>();
static const unsigned place_for_header = common::storage::place_for_data<header>();

writer::writer(std::string const& path, std::string const& sec_code)
    : header_{}
    , path_(path)
    , sec_code_(sec_code) {
    filestream_.exceptions(std::fstream::failbit | std::fstream::badbit);
}

writer::~writer() {}

bool writer::open(){

    bool existed = utils::fs::exists(path_);
    if (!existed) {

        if (sec_code_.empty()) {
            throw std::runtime_error("Empty sec_code is not allowed for a new file");
        }

        utils::fs::mkdir_for_file(path_);

        // init header
        strncpy(header_.sec_code_, sec_code_.c_str(), sizeof(header::sec_code_));
        header_.version_ = 1;
    }

    
    // open a file
    filestream_.open(path_, std::ios_base::out | std::ios_base::binary | (existed ? std::ios::in : 0));

    if (existed)
        read_header();
    else
        update_header();

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

void writer::serialize( messages_view_t messages) {

    using quote_t = common::message::trade::quote;
    using quotes_t = common::message::trade::quotes;


    // File contains many valuable data
    open();

    // no throw no anything - just exit
    if (is_file_compressed())
        return;


    std::vector<char> quotes_raw_data;
    quotes_raw_data.reserve(place_for_quote * messages.size());

    size_t quotes_count = 0;
    for (auto& p : messages) {
        assert(p);

        auto quotes = p.cast<quotes_t>();
        for (auto& quote : quotes->quotes_) {
            common::storage::visit_fields(quote, [&](char* buffer, unsigned size) {
                std::copy(buffer, buffer + size, std::back_inserter(quotes_raw_data));
            });
        }

        quotes_count += quotes->quotes_.size();
    }

    assert(place_for_quote * quotes_count == quotes_raw_data.size());
    if (place_for_quote * quotes_count != quotes_raw_data.size()) {
        std::cerr << "Wrong data. Expected: " << place_for_quote * quotes_count
                  << ". Actual: " << quotes_raw_data.size() << "\n";
        exit(-1);
    }

    // no one trade is add
    if (quotes_raw_data.empty())
        return;

    filestream_.seekp(0, std::fstream::end);
    filestream_.write(quotes_raw_data.data(), quotes_raw_data.size());
    filestream_.flush();

    header_.quotes_count_ += quotes_count;
    update_header();
}

bool writer::is_file_compressed() const {
    return (header_.flags_ & (1 << header::COMPRESSED)) != 0;
}

bool writer::compress() {


    utils::fs::mapped_file mapped_file(path_.c_str());

    // header need for a new file
    auto header_pointer = reinterpret_cast<unsigned char*>(mapped_file.data());
    common::storage::visit_fields(header_, [&, pointer = header_pointer](char* buffer, unsigned size) mutable {
        std::copy(pointer, pointer +size, buffer);
        pointer += size;
    });

    if (is_file_compressed())
        return false;

    // **********************
    //   READING QUOTES
    // **********************
    std::vector<common::storage::quote> deserialized_quotes;

    auto data_pointer   = header_pointer + place_for_header;
    auto file_size      = mapped_file.length();
    auto end_pointer    = header_pointer + file_size;

    assert(place_for_quote * header_.quotes_count_ == end_pointer - data_pointer);

    auto pointer = data_pointer;
    while (pointer != end_pointer) {
        common::storage::quote quote;

        // read buffer and check every field
        common::storage::visit_fields(quote, [&](char* c, unsigned size) {

            if (pointer + size > end_pointer) {
                throw std::runtime_error("Buffer overflow during quotes reading");
            }

            std::copy(pointer, pointer + size, c);
            pointer += size;

        });

        deserialized_quotes.push_back(quote);
    }

    mapped_file.close();
    
    // figure out proper count of quotes
    assert(deserialized_quotes.size() == header_.quotes_count_);

    unsigned deserialized_quotes_size = common::numbers::integer_cast<unsigned>(deserialized_quotes.size() * place_for_quote);

    // ********************

    // ********************
    //   COMPRESSION
    // ********************

    std::vector<unsigned char> compressed_data;
    compressed_data.reserve(deserialized_quotes_size);
    auto compressor = common::storage::quote::make_compressor();
    for (auto& value : deserialized_quotes) {
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
    zipped_file_header.flags_ |= ( 1 << header::COMPRESSED);

    common::storage::visit_fields(zipped_file_header, [&](char* buffer, unsigned size) {
        compressed_file.write(buffer, size);
    });

    compressed_file.write((char*)&original_size, sizeof(original_size));
    compressed_file.write((char*)zipped_data.data(), zipped_data.size());
    compressed_file.close();

    // *********************
    //      CHECKING
    // ********************

    std::vector<common::storage::quote> unzipped_quotes;

    {
        // map and unmap file
        reader r(new_path);
        r.read_all_quotes(unzipped_quotes);
    }

    if (unzipped_quotes.size() != deserialized_quotes.size()) {
        throw std::runtime_error("Unable to compress quotes. Sizes doesn't match.");
    }

    for (unsigned ix = 0; ix < unzipped_quotes.size(); ++ix) {

        auto& before = deserialized_quotes[ix];
        auto& after  = unzipped_quotes[ix];

        utils::apply_tuple(
            [&](auto field) {
                if (before.*field != after.*field) {
                    throw std::runtime_error("Field's values doesn't match.");
                }
            },
            common::storage::quote::fields());
    }

    // ********************
    
    utils::fs::remove_file(path_);
    // move compressed file on it's place
    utils::fs::move(new_path, path_);

    return true;
}
}
} // namespace trade
} // namespace persistent