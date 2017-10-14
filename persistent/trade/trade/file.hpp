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
	unsigned	  version_;
    unsigned char flags_;
    unsigned char reserved_[16]; // possible structure information

	static auto fields() {
        return std::make_tuple(&header::sec_code_, &header::trades_count_, &header::version_,
                               &header::flags_, &header::reserved_);
    }
};

struct file {

	using trades_storage_t = std::vector<common::storage::trade>;

    enum class mode {
        READ = 1,
        WRITE =2
    };

	// binary flags for a header
	enum FLAGS {
		COMPRESSED = 0
	};

    file(std::string const& sec_code, std::string const& storage_folder, uint64_t timestamp, mode m);
    file(std::string const& path); // constructor
    file(const file&) = default;
    file(file&&) = default;
    ~file();

    bool open( );
	
	bool compress();

    header const& file_header() const { return header_; }

    void read_trades(trades_storage_t& buffer, unsigned offset = 0, unsigned max = std::numeric_limits<unsigned>::max());

	void read_header();
	void update_header();

	
private:

	void read_trades_data(trades_storage_t& buffer, unsigned offset, unsigned max);
	void read_trades_compressed(trades_storage_t& buffer, unsigned offset, unsigned max);


protected:
   header       header_;
   std::fstream filestream_;
   mode         mode_;

   std::string sec_code_;
   std::string storage_folder_;
   std::string path_;
};

} // namespace trade
} // namespace trade
} // namespace persistent