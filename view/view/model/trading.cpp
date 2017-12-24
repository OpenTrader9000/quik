
#include "trading.hpp"

#include <view/primitive/bar.hpp>
#include <boost/container/flat_set.hpp>

#include <core/SkCanvas.h>
#include <core/SkPath.h>

#include <string>

#define NOMINMAX

namespace view {
namespace model {

// workaround for compilation.
// TODO: Remove it when all allready be done
trading::trading(std::string const& sec_code, period_t per)
    : datasource_(get_datasource())
    , bar_width_(20)
    , x_left_shift_(0)
    , x_right_shift_(50)
    , y_top_shift_(0)
    , y_bottom_shift_(50)
    , zoom_multiplier_(1.0) {
    sec_code_ = sec_code;
    period_   = per;

    compute_window_name();

    flags_ = Y_CONTINIOUS | X_DISCRETE;
}

trading::~trading() {}

void trading::draw(SkCanvas* canvas, coordinate_t width, coordinate_t height, coordinate_t& x, coordinate_t& y) {
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
    auto count = datasource_->count_of_elements(series_key(sec_code_, period_));
    coordinate_t length_of_canvas = count * bar_width();

    // move cursor to the end
    x = length_of_canvas - width;
    y = 0;
}

void trading::set_property(std::string const& key, std::string const& value) {}

void trading::zoom(float& multiplier, coordinate_t width, coordinate_t height, coordinate_t& x, coordinate_t& y) {

    unsigned bars_total = datasource_->count_of_elements(series_key(sec_code_, period_));

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

std::string const& trading::name() const {
    return window_name_;
}

void trading::compute_window_name() {
    window_name_ = utils::build_string(sec_code_, " - ", persistent::trade::period_str(period_));
}

trading::frame_info trading::compute_frame(coordinate_t width, coordinate_t height, coordinate_t& x,
    coordinate_t& y)
{
    frame_info info;
 
    auto count = datasource_->count_of_elements(series_key(sec_code_, period_));
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
    info.bars_count_ = (x < bar_width() ? count_bars + 1 : count_bars + 2);

    // walk throught every element
    info.values_view_ = datasource_->get_elements(series_key(sec_code_, period_), info.start_index_, info.bars_count_);

    unibcd_t min = common::numbers::max_bcd(info.values_view_[0].low_);
    unibcd_t max = common::numbers::min_bcd(info.values_view_[0].low_);

    for (auto& val : info.values_view_) {
        min = std::min(min, val.low_);
        max = std::max(max, val.high_);
    }

    // generate info for y axis
    info.y_step_ = static_cast<double>(max.value_ - min.value_) / (height - 20);
    info.max_price_ = max;
    info.min_price_ = min;

    info.y_max_.value_           = max.value_ + 10 * info.y_step_;
    info.y_max_.fractional_size_ = max.fractional_size_;
    info.y_max_.sign_            = 0;

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
        primitive::draw_bar(val.open_.value_,
                            val.high_.value_,
                            val.low_.value_,
                            val.close_.value_,
                            canvas,
                            increase,
                            decrease,
                            0,
                            info.y_max_.value_,
                            bar_width(),
                            info.y_step_);
        canvas->translate(bar_width(), 0);
    }
}

std::vector<trading::y_axis_info> trading::legend_info_for_y(frame_info const & info, coordinate_t )
{
    constexpr unsigned minimum_required = 20;

    std::vector<y_axis_info> result;

    // compute size and importance of every scale
    auto size      = info.y_max_.value_ - info.min_price_.value_;
    auto bcd_power = info.max_price_.power();

    unsigned multipliers[] = { 5, 10, 50, 100, 500, 1000, 5000, 10000 };

    int mult_index = -1;
    for (auto mult : multipliers) {
        if (size / mult == 0)
            break;
         ++mult_index;
    }

    // to low count of values
    if (mult_index == -1) {

        y_axis_info ax_info;
        ax_info.importance_ = y_axis_info::LEVEL;
        ax_info.price_      = info.max_price_;

        result.push_back(ax_info);
        return result;
    }

    int end_index = (mult_index > 2 ? mult_index - 3 : -1);
    int important = y_axis_info::LEVEL;

    // prevent dublication
    boost::container::flat_set<uint64_t> existed_values;

    // walk through all possible types of scales
    while (mult_index > end_index) {
        auto mult = multipliers[mult_index];

        unibcd_t value = info.min_price_;
        value.value_   = (value.value_ / mult) * mult; // aligning

        // scales with the same importance
        while (value.value_ < info.y_max_.value_) {
            value.value_ += mult;
            if (existed_values.find(value.value_) != existed_values.end())
                continue;


            y_axis_info ax_info;
            ax_info.importance_ = static_cast<y_axis_info::importance>(important);
            ax_info.price_      = value;

            existed_values.insert(value.value_);
            result.push_back(ax_info);
        }

        --important;
        --mult_index;
    }

    return result;
}


void trading::draw_coordinates(SkCanvas* canvas, frame_info const & info, coordinate_t width,
                               coordinate_t height, coordinate_t& x, coordinate_t& y) {
    auto vals = datasource_->get_elements(series_key(sec_code_, period_), info.start_index_, info.bars_count_);
    
    SkPaint paint, regular_cell_paint, important_cell_paint;
    paint.setColor(SK_ColorBLACK);
    regular_cell_paint.setColor(SK_ColorLTGRAY);
    important_cell_paint.setColor(SK_ColorGRAY);

    unsigned line_length_by_importance[] = { 2,3,6 };
    
    auto additional_botom_place = y_bottom_shift_ / 8;
    auto additional_right_place = x_right_shift_ / 8;

    auto y_position_of_x_axis = height + y_top_shift_ + additional_botom_place;
    auto x_position_of_y_axis = width + x_left_shift_ + additional_right_place;

    // drawing axis y
    canvas->drawLine(x_position_of_y_axis, 0, x_position_of_y_axis, y_position_of_x_axis, paint); // axis y

    auto y_scales = legend_info_for_y(info, height);

    for (auto const& scale : y_scales) {
        auto y_pos = (info.y_max_.value_ - scale.price_.value_) / info.y_step_;
        auto len   = line_length_by_importance[scale.importance_];

        canvas->drawLine(x_position_of_y_axis, y_pos, x_position_of_y_axis + len, y_pos, paint);

        bool is_regular   = (scale.importance_ == y_axis_info::REGULAR);
        bool is_important = (scale.importance_ == y_axis_info::IMPORTANT);
        canvas->drawLine(0, y_pos, x_position_of_y_axis, y_pos, (is_regular || is_important ? regular_cell_paint : important_cell_paint));

        if (is_regular) {
            continue;
        }

        // legend
        auto price_str = scale.price_.to_string();
        canvas->drawText(price_str, strlen(price_str.str), x_position_of_y_axis + len, y_pos + 2, paint);
    }

    // drawing axis x
    canvas->drawLine(0, y_position_of_x_axis, x_position_of_y_axis, y_position_of_x_axis, paint); // axis x
}


} // namespace model
} // namespace view