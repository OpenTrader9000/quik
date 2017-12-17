#pragma once

#include "restorer.hpp"
#include "reader.hpp"
#include "writer.hpp"
#include "order_book_constructor.hpp"

#include <utils/filesystem/filesystem.hpp>
#include <utils/string/build.hpp>

#include <utils/filesystem/filesystem.hpp>
#include <utils/filesystem/mapped_file.hpp>
#include <common/storage/serialize.hpp>
#include <common/message/trade/quote.hpp>
#include <common/message/trade/quotes.hpp>
#include <common/storage/compress/compress.hpp>


#include <iterator>

namespace persistent {
namespace trade {
namespace quote {

void handle_quote(std::string const& path, order_book& last_order_book) {
    std::unique_ptr<order_book_constructor> constructor;

    quotes_sequence_t seq;
    bulk_ptr_t        bulk;
    {
        reader r(path);
        r.read_all_quotes(seq);
        bulk = r.bulk();
    }

    if (seq.size() == 0) {
        return;
    }


    size_t possible_initial_quote_count = 0;

    common::numbers::bcd b = seq.front().price_;

    auto first_ts = seq.front().machine_timestamp_;
    b.value_ -= 1;

    for (auto s : seq) {
        if (b < s.price_)
            possible_initial_quote_count++;
        else
            break;
        b = s.price_;
    }

    // restore order book
    bool has_order_book = (possible_initial_quote_count >= 100);
    if (has_order_book) {
        // update order_book
        constructor = std::make_unique<order_book_constructor>(bulk);
        
        uint64_t day_in_ts = 3600 * 24 * 1000;
        uint64_t end_of_day = (first_ts - first_ts % day_in_ts) + day_in_ts;

        last_order_book = constructor->to_ts(end_of_day).order_book_;

    } else {
        //repaire file
        assert(last_order_book.bid_.size() == 50 && last_order_book.offer_.size() == 50);
        utils::fs::remove_file(path);

        //writer w(path, bulk->sec_code());

        quotes_sequence_t initial;
        for (auto q : last_order_book.bid_) {
            common::storage::quote quot;
            quot.set_bid();
            quot.machine_timestamp_ = first_ts;
            quot.price_ = q.price_;
            quot.quantity_diff_ = q.quantity_;
            initial.push_back(quot);
        }

        for (auto q : last_order_book.offer_) {
            common::storage::quote quot;
            quot.set_offer();
            quot.machine_timestamp_ = first_ts;
            quot.price_ = q.price_;
            quot.quantity_diff_ = q.quantity_;
            initial.push_back(quot);
        }

        // write header
        header h{};
        h.flags_ = 0;
        h.quotes_count_ = initial.size() + seq.size();
        std::copy(bulk->sec_code().begin(), bulk->sec_code().end(), h.sec_code_);
        h.version_ = 1;

        std::vector<char> dat;
        dat.reserve(sizeof(h) + sizeof(common::storage::quote) * (initial.size()+ seq.size()));
        auto write_vis = [&](char* buffer, unsigned size) {
            std::copy(buffer, buffer + size, std::back_inserter(dat));
        };

        common::storage::visit_fields(h, write_vis);
        for (auto& q : initial) {
            common::storage::visit_fields(q, write_vis);
        }

        for (auto& q : seq) {
            common::storage::visit_fields(q, write_vis);
        }

        {
            // write all data
            std::ofstream out(path, std::ios_base::out | std::ios_base::binary);
            out.write(dat.data(), dat.size());
        }

        {
            writer w(path, bulk->sec_code());
            if (!w.compress()) {
                throw "Cannot compress a file " + path;
            }

            bulk.reset();
            initial.clear();
            seq.clear();
        }

        // update all data
        handle_quote(path, last_order_book);
        
    }
}

void restore(std::string const& path) {
    auto quotes = utils::fs::files_in_directory(utils::build_string(path, "/*.ob"));

    order_book last_order_book{};
    for (auto const& q : quotes) {
        auto p = utils::build_string(path, "/", q);
        handle_quote(p, last_order_book);
    }
    
}
} // namespace quote
} // namespace trade
} // namespace persistent