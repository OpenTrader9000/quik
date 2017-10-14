#include "format.hpp"
#include <vector>
#include <utils/process/cli_arguments.hpp>

namespace archtool {
namespace trade {
struct csv_format : public format {

    std::vector<std::string> fields_;
    std::string              buffer_;

    csv_format(utils::cli_arguments const& args);
    ~csv_format();

    virtual void add(common::storage::trade const& trade) override;
    virtual void flush(std::string const& path) override;
    virtual std::string help() override;
};
} // namespace trade
} // namespace archtool