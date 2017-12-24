
#pragma once

#include <core/SkColor.h>
#include <vector>

class SkCanvas;

namespace view {
namespace primitive {
//struct bar {
//
//	enum {
//		HOLLOW = 1
//	};
//
//    bar(double open, double high, double low, double close);
//    ~bar();
//
//    void draw(SkCanvas* canvas, SkColor increase /*green*/, SkColor decrease /*red*/,
//              unsigned flags, double bar_width, double step_height);
//
// private:
//    double open_;
//    double high_;
//    double low_;
//    double close_;
//};

void draw_bar(uint64_t  open,
              uint64_t  high,
              uint64_t  low,
              uint64_t  close,
              SkCanvas* canvas,
              SkColor   increase /*green*/,
              SkColor   decrease /*red*/,
              unsigned  flags,
              uint64_t  top,
              double    bar_width,
              double    step_height);

//struct reversed_bars {
//	std::vector<bar>	bars_;
//    unsigned            shift_;
//};
} // namespace primitive
} // namespace view