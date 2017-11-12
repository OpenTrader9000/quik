#pragma once

#include <memory>
#include <unordered_map>

class SkCanvas;

namespace view {
namespace model {

struct model {

public:
	using coordinate_t = unsigned;
	using properties_t = std::unordered_map<std::string, std::string>;

protected:

   enum axis_type {
       Y_DISCRETE   = 1,
       Y_CONTINIOUS = 2,
       X_DISCRETE   = 4,
       X_CONTINIOUS = 8,
   };


   unsigned     flags_;
   unsigned     x_step_size_in_pixels_;
   unsigned     y_step_size_in_pixels_;
   properties_t properties_;

public:


    model();
    virtual ~model();

	// continuous or discrete
	bool is_x_discrete() const;
	bool is_y_discrete() const;

	bool is_x_continuous() const;
	bool is_y_continuous() const;

	unsigned x_step_size_in_pixels() const;
	unsigned y_step_size_in_pixels() const;

	properties_t const&  properties() const;

    virtual void draw(SkCanvas* canvas, coordinate_t width, coordinate_t height, coordinate_t& x,
                      coordinate_t& y)                                          = 0;
    virtual void set_property(std::string const& key, std::string const& value) = 0;
    virtual bool new_data_availiable() const;
};

} // namespace model

using model_ptr_t = std::shared_ptr<model::model>;
} // namespace view