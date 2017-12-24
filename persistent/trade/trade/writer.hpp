#pragma once

#include <fstream>
#include <persistent/pch.hpp>
#include <common/storage/trade.hpp>
#include "file.hpp"


namespace persistent {
namespace trade {
namespace trade {


struct writer {

	using trades_storage_t = std::vector<common::storage::trade>;
    //using trades_view_t = common::container::array_view<common::storage::trade>;

    writer(std::string const& path, std::string const& sec_code); // constructor
    writer(const writer&) = default;
    writer(writer&&) = default;
    ~writer();

    bool open( );
	
    // false - operation doesn't required
	bool compress();

    header const& file_header() const { return header_; }
    
    void serialize(messages_view_t messages);

private:
    void read_header();
    void update_header();

    bool is_file_compressed();

    void filter(trades_storage_t& trades);


protected:
   header       header_;
   std::fstream filestream_;

   std::string sec_code_;
   std::string path_;
};

} // namespace trade
} // namespace trade
} // namespace persistent