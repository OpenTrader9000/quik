#pragma once

#include "model.hpp"
#include "datasource.hpp"

#include <ohlc.hpp>
#include <common/container/array_view.hpp>
#include <view/primitive/bar.hpp>

namespace view {
namespace model {

//struct datasource {
//
//	// TODO: Move it to persistent or common
//	using ohlc_array_t = std::vector<ohlc>;
//	datasource(ohlc_array_t const& values);
//	~datasource();
//
//	// function return bars by parameters
//	// count - count of extracted elements
//	common::container::array_view<ohlc> get_elements(unsigned start_index, unsigned count);
//    common::container::array_view<ohlc> get_elements_starts_with_index(unsigned start_index);
//
//    size_t count_of_elements() {
//        return values_.size();
//    }
//
//private:
//	ohlc_array_t values_;
//};

using datasource_ptr_t = std::shared_ptr<datasource>;

struct trading : public model {

	trading(std::string const& sec_code, period_t per);
	~trading();

    virtual void draw(SkCanvas* canvas, coordinate_t width, coordinate_t height, coordinate_t& x,
                      coordinate_t& y) override;

    virtual void init(coordinate_t width, coordinate_t height, coordinate_t& x, coordinate_t& y) override;

    virtual void set_property(std::string const& key, std::string const& value) override;
    virtual void zoom(float& multiplier, coordinate_t width, coordinate_t height, coordinate_t& x,
                      coordinate_t& y) override;
    virtual std::string const& name() const override;

 private:

     void compute_window_name();

    // TODO: Remove with structured binding frame_info


    struct frame_info {
        unsigned start_index_;
        unsigned bars_count_;
        float    start_position_;

        unibcd_t      min_price_;
        unibcd_t      max_price_;
        double        y_step_; // step of one integer change value in pixels
        unibcd_t      y_max_;
        ohlc_view_t   values_view_;
    };

    frame_info compute_frame(coordinate_t width, coordinate_t height, coordinate_t& x, coordinate_t& y);

    void draw_coordinates(SkCanvas* canvas, frame_info const& info, coordinate_t width,
                          coordinate_t height, coordinate_t& x, coordinate_t& y);

    void draw_bars(SkCanvas* canvas, frame_info const& info, coordinate_t width,
                   coordinate_t height, coordinate_t& x, coordinate_t& y);

    float bar_width() { return bar_width_ * zoom_multiplier_; }

    // TODO: Move coordinate drawing in another translation module

    // special info for y coordinate show price and it importance
    struct y_axis_info {

        enum importance {
            REGULAR   = 0,
            IMPORTANT = 1,
            LEVEL     = 2,
        };

        unibcd_t   price_;
        importance importance_;
    };

    std::vector<y_axis_info> legend_info_for_y(frame_info const& info, coordinate_t height);

    struct x_axis_info {
        enum ts_type {
            minute = 0,
            hour   = 1,
            day    = 2,
           // week   = 3,
        };

        uint64_t   time_;
        ts_type    ts_type_;
        unsigned   bar_index_in_view_;
    };

    std::vector<x_axis_info> legend_info_for_x(frame_info const& info, coordinate_t width);


    // coordinates start and end
    coordinate_t     x_left_shift_;
    coordinate_t     x_right_shift_;
    coordinate_t     y_top_shift_;
    coordinate_t     y_bottom_shift_;
    datasource_ptr_t datasource_;
    float            bar_width_;
    float            zoom_multiplier_;

    std::string     sec_code_;
    period_t        period_;
    std::string     window_name_;
};

} // namespace model
} // namespace view