#include "bar.hpp"

#include <core/SkPaint.h>
#include <core/SkCanvas.h>
#include <core/SkPath.h>
#include <core/SkColorFilter.h>

namespace view {
namespace primitive {

//bar::bar(double open, double high, double low, double close)
//: open_(open)
//, high_(high)
//, low_(low)
//, close_(close) {}
//
//bar::~bar() {}
//
//void bar::draw(SkCanvas* canvas, SkColor increase /*green*/, SkColor decrease /*red*/,
//               unsigned flags, double bar_width, double step_height) {
//    SkPaint paint;
//}


void draw_bar(uint64_t  open,
              uint64_t  high,
              uint64_t  low,
              uint64_t  close,
              SkCanvas* canvas,
              SkColor   increase_color,
              SkColor   decrease_color,
              unsigned  flags,
              uint64_t  top,
              double    bar_width,
              double    step_height) {
	
	constexpr SkScalar stroke_width = 1;

	bool is_increase = (close > open);

	SkPaint paint;
	paint.setStrokeWidth(stroke_width);
	paint.setColor(SK_ColorBLACK);
	paint.setStyle(SkPaint::kStroke_Style);
	//paint.setColorFilter(SkColorFilter::MakeLinearToSRGBGamma());

	
	//paint.setAntiAlias(true);
	//paint.setColor(SK_ColorRED);

	auto open_close_high = (is_increase ? close : open);
    auto open_close_low  = (is_increase ? open : close);

    auto figure_top    = (top - high) / step_height;
    auto figure_bottom = (top - low) / step_height;
	auto fitted_figure_width = bar_width * 0.8;

	auto figure_rect_top = (top - open_close_high) / step_height;
	auto figure_rect_bottom = (top - open_close_low) / step_height;

	if (figure_rect_bottom == figure_rect_top)
		figure_rect_bottom += 1;

	SkRect rect;
	rect.set(-fitted_figure_width / 2, figure_rect_top, fitted_figure_width / 2, figure_rect_bottom);

	SkPaint fill_paint;
	fill_paint.setColor(is_increase ? increase_color : decrease_color);

	canvas->drawRect(rect, fill_paint);

    //SkPath path;
    canvas->drawLine(0, figure_top,			0, figure_rect_top, paint);
	canvas->drawLine(0, figure_rect_bottom, 0, figure_bottom, paint);
	canvas->drawRect(rect,  paint);
	

}

} // namespace primitive
} // namespace view