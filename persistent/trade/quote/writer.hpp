#pragma once

#include <fstream>
#include <persistent/pch.hpp>
#include <common/storage/quote.hpp>

#include "file.hpp"

namespace persistent {
namespace trade {
namespace quote {

// structure write all values in the file and compress them in the end
struct writer {

    writer(std::string const& path, std::string const& sec_code = ""); // constructor for read operations
    writer(const writer&) = default;
    writer(writer&&) = default;
    ~writer();

    bool open( );
	
    header const& file_header() const { return header_; }

    // write quotes with proper sec_code into a file
    void serialize(messages_view_t messages);

    bool is_file_compressed() const;
    bool compress();
	
	
private:
    void update_header();
    void read_header();

protected:
   header       header_;
   std::fstream filestream_;
   //mode         mode_;

   //std::string storage_folder_;
   std::string path_;
   std::string sec_code_;

};

} // namespace trade
} // namespace trade
} // namespace persistent