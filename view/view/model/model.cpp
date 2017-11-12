#include "model.hpp"


namespace view {
namespace model {
model::model()
: flags_(0)
, x_step_size_in_pixels_(0)
, y_step_size_in_pixels_(0) {}

model::~model() {}
bool model::is_x_discrete() const {
    return (flags_ & X_DISCRETE) != 0;
}
bool model::is_y_discrete() const {
    return (flags_ & Y_DISCRETE) != 0;
    ;
}
bool model::is_x_continuous() const {
    return (flags_ & X_CONTINIOUS) != 0;
}
bool model::is_y_continuous() const {
    return (flags_ & Y_CONTINIOUS) != 0;
}
unsigned model::x_step_size_in_pixels() const {
    return x_step_size_in_pixels_;
}
unsigned model::y_step_size_in_pixels() const {
    return y_step_size_in_pixels_;
}
model::properties_t const& model::properties() const {
    return properties_;
}
bool model::new_data_availiable() const {
    return false;
}
} // namespace model
} // namespace view