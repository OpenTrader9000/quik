
#include "trading.hpp"

#include <view/primitive/bar.hpp>

#include <core/SkCanvas.h>
#include <core/SkPath.h>

#include <string>

namespace view {
namespace model {

trading::trading(datasource_ptr_t ds)
: datasource_(ds)
, bar_width_(20)
, x_left_shift_(0)
, x_right_shift_(50)
, y_top_shift_(0)
, y_bottom_shift_(50)
, zoom_multiplier_(1.0)
{
    flags_ = Y_CONTINIOUS | X_DISCRETE; 
}

trading::~trading() {}

void trading::draw(SkCanvas* canvas, coordinate_t width, coordinate_t height, coordinate_t& x,
                   coordinate_t& y) {
	canvas->drawColor(SK_ColorWHITE);

	// cannot draw anithing
	if (height < 30)
		return;

    width  = width - (x_right_shift_ + x_left_shift_);
    height = height - (y_top_shift_ + y_bottom_shift_);

    // all information that needed for painting a frame
    auto frm_info = compute_frame(width, height, x, y);

    draw_coordinates(canvas, frm_info, width, height, x, y);
    draw_bars(canvas, frm_info, width, height, x, y);

}

void trading::init(coordinate_t width, coordinate_t height, coordinate_t& x, coordinate_t& y) {
    auto count = datasource_->count_of_elements();
    coordinate_t length_of_canvas = count * bar_width();

    // move cursor to the end
    x = length_of_canvas - width;
    y = 0;
}

void trading::set_property(std::string const& key, std::string const& value) {}

void trading::zoom(float& multiplier, coordinate_t width, coordinate_t height, coordinate_t& x, coordinate_t& y) {

    unsigned bars_total = datasource_->count_of_elements();

    width -= x_left_shift_ + x_right_shift_;

    multiplier = (bars_total * multiplier * bar_width_ > width ? multiplier : static_cast<float>(width) / (bars_total*bar_width_));

    // old bars info
    float old_bar         = zoom_multiplier_ * bar_width_;
    unsigned old_first_index = static_cast<unsigned>(x / old_bar);
    unsigned old_bars_count  = static_cast<unsigned>(width / old_bar);

    float new_bar        = multiplier * bar_width_;
    unsigned new_bars_count = static_cast<unsigned>(width / new_bar);

    unsigned index = 0;
    int needed_bars = static_cast<int>(new_bars_count) - static_cast<int>(old_bars_count);
    if (needed_bars > 0) {
        unsigned availiable_on_the_left  = old_first_index;
        index = (availiable_on_the_left > needed_bars / 2 ? old_first_index - needed_bars / 2 : 0);
    }
    else {
        index = old_first_index + (-needed_bars) / 2;
    }
    
    // setup position
    x = new_bar * index;

    zoom_multiplier_ = multiplier;
}

trading::frame_info trading::compute_frame(coordinate_t width, coordinate_t height, coordinate_t& x,
    coordinate_t& y)
{
    frame_info info;
 
    auto count = datasource_->count_of_elements();
    auto count_bars = width / bar_width();

    auto width_for_bars = count * bar_width();
    if (x + width > width_for_bars) {
        if (width < width_for_bars) {
            x = width_for_bars - width;
        } else {
            x = 0;
        }
    }

    // compute start index.
    // it most be leftest bar 
    info.start_index_ = static_cast<unsigned>(x / bar_width());
    info.start_index_ = (info.start_index_ > 0 ? info.start_index_ - 1 : info.start_index_);

    // position for drawing. Must be negative for case when the first bar outside left side of the canvas.
    info.start_position_ = (static_cast<float>(info.start_index_ * bar_width()) - x) + bar_width() / 2 + x_left_shift_;
    info.bars_count_ = ( x < bar_width() ? count_bars + 1 : count_bars + 2);

    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();

    // walk throught every element
    info.values_view_ = datasource_->get_elements(info.start_index_, info.bars_count_);
    for (auto& val : info.values_view_) {
        min = std::fmin(min, val.low_);
        max = std::fmax(max, val.high_);
    }

    // generate info for y axis
    info.y_step_ = (max - min) / (height - 20);
    info.y_max_ = max + 10 * info.y_step_;
    info.max_price_ = max;
    info.min_price_ = min;

    return info;
}

void trading::draw_bars(SkCanvas* canvas, frame_info const& info, coordinate_t width,
                        coordinate_t height, coordinate_t& x, coordinate_t& y) {

    y = 0;

    SkPaint paint;
    paint.setColor(SK_ColorRED);

    SkColor increase = SkColorSetARGB(255, 130, 255, 130);
    SkColor decrease = SkColorSetARGB(255, 255, 130, 130);

    canvas->resetMatrix();
    canvas->translate(info.start_position_, y_top_shift_);
    for (auto& val : info.values_view_) {
        primitive::draw_bar(val.open_,
                            val.high_,
                            val.low_,
                            val.close_,
                            canvas,
                            increase,
                            decrease,
                            0,
                            info.y_max_,
                            bar_width(),
                            info.y_step_);
        canvas->translate(bar_width(), 0);
    }
}

std::pair<unsigned, unsigned> trading::legend_info_for_y(frame_info const & info, coordinate_t )
{
    constexpr unsigned minimum_required = 20;

    unsigned multipliers[] = { 5,10,20,40,50,80,100,200,400,500,1000 }; // very impossible values
    unsigned step = 0;
    for (auto mult : multipliers) {
        step = static_cast<unsigned>(info.y_step_ * mult);
        if (step > minimum_required)
            break;
    }

    return std::pair<unsigned, unsigned>(step, minimum_required/2);
}

std::pair<unsigned, unsigned> trading::legend_info_for_x(frame_info const & info, coordinate_t width)
{
    constexpr unsigned minimum_required = 30;
    unsigned multipliers[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50 }; 
    // compute single step 
    auto single_step = width / info.values_view_.size();
    
    unsigned step;
    for (auto mult : multipliers) {
        step = single_step * mult;
        if (step > minimum_required)
            break;
    }

    return std::pair<unsigned, unsigned>(step,1);
}

void trading::draw_coordinates(SkCanvas* canvas, frame_info const & info, coordinate_t width,
                               coordinate_t height, coordinate_t& x, coordinate_t& y) {
    auto vals = datasource_->get_elements(info.start_index_, info.bars_count_);
    
    SkPaint paint;
    paint.setColor(SK_ColorBLACK);
    auto y_position_of_x_axis = height + y_top_shift_ + y_bottom_shift_ / 4;
    auto x_position_of_y_axis = width + x_left_shift_ + x_right_shift_ / 4;

    // drawing axis y
    canvas->drawLine(x_position_of_y_axis, 0, x_position_of_y_axis, y_position_of_x_axis, paint); // axis y

    auto step_position_pair = legend_info_for_y(info,height);
    auto y_legend_step = step_position_pair.first;
    auto first_price = info.y_max_ - step_position_pair.second*info.y_step_;
    for (auto price = first_price; price > info.min_price_; price -= y_legend_step) {

        // marks
        auto y_pos = (info.y_max_ - price) / info.y_step_;
        canvas->drawLine(x_position_of_y_axis, y_pos, x_position_of_y_axis + 3, y_pos, paint);

        // legend
        auto price_str = std::to_string(static_cast<unsigned>(price));
        canvas->drawText(price_str.c_str(), price_str.size(), x_position_of_y_axis + 3, y_pos +2, paint);
    }

    // drawing axis x
    canvas->drawLine(0, y_position_of_x_axis, x_position_of_y_axis, y_position_of_x_axis, paint); // axis x
    //auto x_step_position = legend_info_for_x(info, width);
    //auto x_step          = x_step_position.first;
    //auto x_first         = x_step_position.second;



    //auto max_price_str = std::to_string(static_cast<unsigned>(info.max_price_));
    //auto min_price_str = std::to_string(static_cast<unsigned>(info.min_price_));
    //canvas->drawText(max_price_str.c_str(), max_price_str.size(), x_position_of_y_axis + 3, 10, paint);
    //canvas->drawText(min_price_str.c_str(), min_price_str.size(), x_position_of_y_axis + 3, y_position_of_x_axis - 10, paint);

}

datasource::datasource(ohlc_array_t const& values)
: values_(values) {}

datasource::~datasource() {}

common::container::array_view<ohlc> datasource::get_elements(unsigned start_index, unsigned count) {
    if (start_index + count >= values_.size()) {
        count = values_.size() - start_index;
    } 
    return common::container::array_view<ohlc>(values_.data() + start_index, count);
}

common::container::array_view<ohlc> datasource::get_elements_starts_with_index(unsigned start_index)
{
    if (start_index > values_.size())
        return common::container::array_view<ohlc>();
    unsigned count = values_.size() - start_index;
    return common::container::array_view<ohlc>(values_.data() + start_index, count);
}

} // namespace model
} // namespace view