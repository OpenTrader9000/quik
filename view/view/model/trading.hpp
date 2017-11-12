#pragma once

#include "model.hpp"

#include <ohlc.hpp>


namespace view {
namespace model {

struct datasource {

	// TODO: Move it to persistent or common
	using ohlc_array_t = std::vector<ohlc>;
	datasource(ohlc_array_t const& values);
	~datasource();

private:
	ohlc_array_t values_;
};

using datasource_ptr_t = std::shared_ptr<datasource>;

struct trading : public model {

	trading();
	~trading();

    virtual void draw(SkCanvas* canvas, coordinate_t width, coordinate_t height, coordinate_t& x,
                      coordinate_t& y) override;

    virtual void set_property(std::string const& key, std::string const& value) override;
};
} // namespace model
} // namespace view