#include "file.hpp"

#include <utils/string/build.hpp>
#include <iomanip>
#include <unordered_set>
#include <time.h>

#include <utils/filesystem/filesystem.hpp>
#include <common/storage/serialize.hpp>

namespace persistent {
namespace trade {
namespace quote {

file::file(std::string const& sec_code, std::string const& storage_folder, uint64_t timestamp, mode m)
: header_{}
, mode_(m)
, sec_code_(sec_code)
, storage_folder_(storage_folder) {

    // compute time
    time_t    t = static_cast<time_t>(timestamp);
    struct tm time;
    ::localtime_s(&time, &t);

    // sec_code/year/month/dd.ob
    // RIU7/2017/10/07.ob
    path_ = utils::build_string(storage_folder_, '/', 1900 + time.tm_year, '/', sec_code_, '/',
                                1900 + time.tm_year, std::setw(2), std::setfill('0'), 1 + time.tm_mon,
                                std::setw(2), std::setfill('0'), time.tm_mday, ".ob");

    std::cerr << "FilePath: " << path_ << "\t" << timestamp << "\n";
}

file::file(std::string const& path)
: path_(path)
, mode_(mode::READ) {}

file::~file() {}

bool file::open(){

    if (!utils::fs::exists(path_)) {
        if (mode_ == mode::READ){
            return false;
        }

        utils::fs::mkdir_for_file(path_);

        // init header
        strncpy(header_.sec_code_, sec_code_.c_str(), sizeof(header::sec_code_));
		header_.version_ = 1;

        // open and write header
        filestream_.open(path_,  std::fstream::out | std::ios_base::binary);
        if (!filestream_.is_open()) {
            return false;
        }

		update_header();
    }
	else {

		// open a file
        int mode = (mode_ == mode::WRITE ? std::ios_base::out |  std::ios_base::in : std::ios_base::in ) |
                   std::ios_base::binary;
        filestream_.open(path_, mode);
		if (!filestream_.is_open()) {
			return false;
		}
		
		read_header();
	}

    return true;
    
}

//bool file::compress() {
//	assert(filestream_.is_open());
//
//}

void file::read_quotes_data(quotes_storage_t& buffer, unsigned offset, unsigned max){

    using quote_t = common::storage::quote;

	unsigned quote_size = common::storage::place_for_data<quote_t>();
	unsigned header_size = common::storage::place_for_data<header>();

    // compute count to read
    uint64_t quotes_to_read = header_.quotes_count_ - offset;
    quotes_to_read = (quotes_to_read > max ? max : quotes_to_read);

    // prepare buffer
	filestream_.seekg(0, std::ios::end);
	auto last = filestream_.tellg();

	// compute size for reading
	auto position = header_size + offset * quote_size;
	auto allocate_size = (header_.quotes_count_ - offset) * quote_size;

	// check file format 
	assert((unsigned)last - position == allocate_size);
	std::vector<char> tmp_buffer(allocate_size);

    // read
    filestream_.seekg(position);
    filestream_.read(tmp_buffer.data(), tmp_buffer.size());

	// read data by element
	char* begin = tmp_buffer.data();
	char* end = begin + tmp_buffer.size();
	while (begin != end) {
        quote_t quote;
        
		// read buffer and check every field
		common::storage::visit_fields(quote, [&](char* c, unsigned size) {

			if (begin + size > end) {
				throw std::runtime_error("Buffer overflow issue");
			}

			std::copy(begin, begin + size, c);
			begin += size;

		});

        buffer.push_back(quote);
    }
}

void file::read_header() {
	filestream_.seekg(0);

	common::storage::visit_fields(header_, [&](char* buffer, unsigned size) {
		filestream_.read(buffer, size);
	});
}

void file::update_header()
{

	std::cerr << "Write ts:" <<  "\tcount:" << header_.quotes_count_ << "\n";

	filestream_.seekp(0);
	common::storage::visit_fields(header_, [&](char* buffer, unsigned size) {
		filestream_.write(buffer, size);
	});

}

void file::read_quotes(quotes_storage_t & buffer, unsigned offset, unsigned max)
{

	assert(filestream_.is_open());

	if (offset >= header_.quotes_count_) {
		buffer.clear();
		return;
	}

	read_quotes_data(buffer, offset, max);
}

void file::read_quotes_compressed(quotes_storage_t & buffer, unsigned offset, unsigned max)
{
}


}
} // namespace trade
} // namespace persistent