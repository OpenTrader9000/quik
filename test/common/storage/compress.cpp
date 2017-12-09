#include <catch.hpp>

#include <common/storage/compress/make.hpp>
#include <common/storage/compress/compress.hpp>
#include <vector>
#include <tuple>

// prepare data for tests
struct compressable_struct {
    unsigned index_;
    uint64_t time_;
    int64_t  some_stuff_;
    unsigned char flags_;
    int64_t  price_;
    common::numbers::bcd price2_;

    bool operator==(compressable_struct const& other) const {
        return index_ == other.index_ && time_ == other.time_ && some_stuff_ == other.some_stuff_ &&
               flags_ == other.flags_ && price_ == other.price_ && price2_ == other.price2_;
    }

    friend std::ostream& operator << (std::ostream& os, compressable_struct const& v) {
        os << "{" << v.index_ << "," << v.time_ << "," << v.some_stuff_ << "," << (int) v.flags_ << "," << v.price_ << "," << v.price2_.to_string() << "}";
        return os;
    }
};

static const std::vector<compressable_struct> compressable_example =
    { { 0, 1511622536, -1, 12, 52000      , "52000.12" },      { 1, 1511622546, -2, 12, 51000  , "51200.22" },
      { 2, 1511622546, -4, 12, 51550      , "52100.12" },      { 3, 1511622556, 60, 12, 52300  , "51300.22" },
      { 4, 1511622566, -120, 12, 52400    , "52300.12" },    { 5, 1511622576, -400, 12, 52100  , "51400.22" },
      { 6, 1511622586, -500, 12, 52000    , "52500.12" },    { 7, 1511622596, 50000, 12, 52900 , "51500.22" },
      { 8, 1511622336, -10000, 12, 52500  , "52600.12" },  { 9, 1511622536, -100000, 12, 52300 , "51600.22" },
      { 10, 1511622636, 900100, 12, 52400 , "52300.12" }, { 11, 1511622736, 10500, 12, 52500   , "51700.22" } };

inline auto make_compressor() {
    using namespace common::storage::compress;
    return std::make_tuple(history_compressor(&compressable_struct::index_),
                           history_compressor(&compressable_struct::time_),
                           compressor(&compressable_struct::some_stuff_),
                           compressor(&compressable_struct::flags_),
                           history_compressor(&compressable_struct::price_),
                           history_compressor(&compressable_struct::price2_));
}


TEST_CASE("Compress", "Check compression and decompression"){

    // compress start
    std::vector<unsigned char> compressed_data;
    auto compressor = make_compressor();

    for (auto& value : compressable_example) {
        common::storage::compress::encode(value, compressor, [&](common::container::array_view<unsigned char> view) {
            std::copy(view.begin(), view.end(), std::back_inserter(compressed_data));
        });
    }

    std::vector<compressable_struct> other;
    unsigned char const* begin = compressed_data.data();
    unsigned char const* end = begin + compressed_data.size();
    unsigned size = static_cast<unsigned>(compressed_data.size());

    while (begin != end) {
        compressable_struct instance;
        common::storage::compress::decode(instance, compressor, begin, size);
        other.push_back(instance);
    }

    REQUIRE(compressable_example == other);
}