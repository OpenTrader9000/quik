#include "render.hpp"
#include "view.hpp"

namespace view {

static std::shared_ptr<render_client> g_window;
std::shared_ptr<render_client>        main_window() {
    return g_window;
}

void init_render() {
    g_window = std::make_shared<view>();
}
} // namespace view