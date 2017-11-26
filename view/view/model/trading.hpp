#pragma once

#include "model.hpp"

#include <ohlc.hpp>
#include <common/container/array_view.hpp>
#include <view/primitive/bar.hpp>

namespace view {
namespace model {

struct datasource {

	// TODO: Move it to persistent or common
	using ohlc_array_t = std::vector<ohlc>;
	datasource(ohlc_array_t const& values);
	~datasource();

	// function return bars by parameters
	// count - count of extracted elements
	common::container::array_view<ohlc> get_elements(unsigned start_index, unsigned count);
    common::container::array_view<ohlc> get_elements_starts_with_index(unsigned start_index);

    size_t count_of_elements() {
        return values_.size();
    }

private:
	ohlc_array_t values_;
};

using datasource_ptr_t = std::shared_ptr<datasource>;

struct trading : public model {

	trading(datasource_ptr_t source);
	~trading();

    virtual void draw(SkCanvas* canvas, coordinate_t width, coordinate_t height, coordinate_t& x,
                      coordinate_t& y) override;

    virtual void init(coordinate_t width, coordinate_t height, coordinate_t& x, coordinate_t& y) override;

    virtual void set_property(std::string const& key, std::string const& value) override;
    virtual void zoom(float& multiplier, coordinate_t width, coordinate_t height, coordinate_t& x,
                      coordinate_t& y) override;

 private:

    // TODO: Remove with structured binding frame_info


    struct frame_info {
        unsigned start_index_;
        unsigned bars_count_;
        float    start_position_;

        double                            min_price_;
        double                            max_price_;
        double                              y_step_;
        double                              y_max_;
        common::container::array_view<ohlc> values_view_;
    };

    frame_info compute_frame(coordinate_t width, coordinate_t height, coordinate_t& x, coordinate_t& y);

    void draw_coordinates(SkCanvas* canvas, frame_info const& info, coordinate_t width,
                          coordinate_t height, coordinate_t& x, coordinate_t& y);

    void draw_bars(SkCanvas* canvas, frame_info const& info, coordinate_t width,
                   coordinate_t height, coordinate_t& x, coordinate_t& y);

    float bar_width() { return bar_width_ * zoom_multiplier_; }

    // first parameter step
    // second parameter is first position
    std::pair<unsigned,unsigned> legend_info_for_y(frame_info const& info, coordinate_t height);
    std::pair<unsigned,unsigned> legend_info_for_x(frame_info const& info, coordinate_t width);

    

    // coordinates start and end
    coordinate_t     x_left_shift_;
    coordinate_t     x_right_shift_;
    coordinate_t     y_top_shift_;
    coordinate_t     y_bottom_shift_;
    datasource_ptr_t datasource_;
    float            bar_width_;
    float            zoom_multiplier_;
};

} // namespace model
} // namespace view