#pragma once

#include <fstream>
#include <persistent/pch.hpp>
#include <common/storage/trade.hpp>


namespace persistent {
namespace trade {
namespace trade {

struct header {
    char          sec_code_[16];
    unsigned      trades_count_;
	uint64_t      last_timestamp_in_ms_;
	unsigned	  version_;
    unsigned char flags_;
    unsigned char reserved_[16]; // possible structure information
};

struct file {

    enum class mode {
        READ = 1,
        WRITE =2,
    };

    header          header_;
    std::fstream    filestream_;

    std::string   sec_code_;
    std::string   storage_folder_;

    mode          mode_;

	std::string path_;

    file(std::string const& sec_code, std::string const& storage_folder, mode m);
    file(const file&) = default;
    file(file&&) = default;
    ~file();

    bool open( uint64_t timestamp);

    void read_trades(std::vector<common::storage::trade>& buffer, unsigned offset = 0, unsigned max = std::numeric_limits<unsigned>::max());

	void update_header();

    //void update_trades_count();
    //void update_timestamp();


    
};

} // namespace trade
} // namespace trade
} // namespace persistent