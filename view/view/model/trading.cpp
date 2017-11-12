
#include "trading.hpp"

#include <core/SkCanvas.h>

namespace view {
namespace model {

trading::trading() {
    flags_ = Y_CONTINIOUS | X_DISCRETE;
}

trading::~trading() {}

void trading::draw(SkCanvas* canvas, coordinate_t width, coordinate_t height, coordinate_t& x,
                   coordinate_t& y) {
	canvas->drawColor(SK_ColorWHITE);

}

void trading::set_property(std::string const& key, std::string const& value) {}

datasource::datasource(ohlc_array_t const& values)
: values_(values) {}

datasource::~datasource() {}

} // namespace model
} // namespace view