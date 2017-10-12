#include "file.hpp"

#include <utils/string/build.hpp>
#include <iomanip>
#include <time.h>

#include <utils/filesystem/filesystem.hpp>
#include <common/storage/serialize.hpp>

namespace persistent {
namespace trade {
namespace trade {

file::file(std::string const& sec_code, std::string const& storage_folder, mode m)
: header_{}
, sec_code_(sec_code)
, storage_folder_(storage_folder)
, mode_(m) {}

file::~file() {}

bool file::open(uint64_t timestamp){
    // compute time
    time_t t = static_cast<time_t>(timestamp);
    struct tm time;
    ::localtime_s(&time, &t);

    // sec_code/year/month/dd.trd
    // RIU7/2017/10/07.trd
    path_ = utils::build_string(storage_folder_, '/', sec_code_, '/', 1900 + time.tm_year, 
                                    std::setw(2), std::setfill('0'), 1 + time.tm_mon,
                                    std::setw(2), std::setfill('0'), time.tm_mday, ".trd");

    std::cerr << "FilePath: " << path_ << "\t" << timestamp << "\n";

    if (!utils::fs::exists(path_)) {
        if (mode_ == mode::READ){
            return false;
        }

        utils::fs::mkdir_for_file(path_);

        // init header
        strncpy(header_.sec_code_, sec_code_.c_str(), sizeof(header::sec_code_));
		header_.version_ = 1;

        // open and write header
        filestream_.open(path_,  std::fstream::out );
        if (!filestream_.is_open()) {
            return false;
        }

        filestream_.write(reinterpret_cast<char const*>(&header_), sizeof(header_));
    }
	else {

		// open a file
        int mode = (mode_ == mode::WRITE ? std::ios_base::out |  std::ios_base::in : std::ios_base::in ) |
                   std::ios_base::binary;
        filestream_.open(path_, mode);
		if (!filestream_.is_open()) {
			return false;
		}

		// read header
		filestream_.seekg(0);
		filestream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
	}

    return true;
    
}

void file::read_trades(std::vector<common::storage::trade>& buffer, unsigned offset, unsigned max){

    assert(filestream_.is_open());

    if (offset >= header_.trades_count_) {
        buffer.clear();
        return;
    }

    // compute count to read
    unsigned trades_to_read = header_.trades_count_ - offset;
    trades_to_read = (trades_to_read > max ? max : trades_to_read);

    // prepare buffer
    //buffer.resize(trades_to_read);

	//filestream_.seekg(0, std::ios::beg);
	//auto first = filestream_.tellg();
	filestream_.seekg(0, std::ios::end);
	auto last = filestream_.tellg();

	auto position = sizeof(header) - offset * sizeof(common::storage::trade);
	std::vector<char> tmp_buffer((int)last -  position);
    // read
    filestream_.seekg(position);
    filestream_.read(tmp_buffer.data(), tmp_buffer.size());

	// read data by element
	char* begin = tmp_buffer.data();
	char* end = begin + tmp_buffer.size();
	while (begin != end) {
		common::storage::trade trade;
        common::storage::deserialize(begin, trade.flags_, trade.price_, trade.machine_timestamp_,
                                     trade.server_timestamp_, trade.quantity_, trade.open_interest_);
        buffer.push_back(trade);
    }
}

void file::update_header()
{

	//filestream_.close();

	std::cerr << "Write ts:" << header_.last_timestamp_in_ms_ << "\tcount:" << header_.trades_count_ << "\n";

	//std::ofstream out(path_, std::ios_base::binary | std::ios::app);
	filestream_.seekp(0);
	filestream_.write(reinterpret_cast<char const*>(&header_), sizeof(header));
}

//void file::update_trades_count(){
//	
//    int position = reinterpret_cast<char*>(&header_.trades_count_) - reinterpret_cast<char*>(&header_);
//    std::cerr << "Write trades count " << header_.trades_count_ << " at position " << position << "\n";
//    filestream_.seekp(position);
//    filestream_.write(reinterpret_cast<char const*>(&header_.trades_count_), sizeof(header_.trades_count_));
//}
//
//void file::update_timestamp(){
//
//    int position = reinterpret_cast<char*>(&header_.last_timestamp_in_ms_) - reinterpret_cast<char*>(&header_);
//    std::cerr << "Write last timestamp " << header_.last_timestamp_in_ms_ << " at position " << position << "\n";
//
//    filestream_.seekp(position);
//    filestream_.write(reinterpret_cast<char const*>(&header_.last_timestamp_in_ms_), sizeof(header_.last_timestamp_in_ms_));
//
//}

}
} // namespace trade
} // namespace persistent