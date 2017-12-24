#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "render.hpp"
#include "model/datasource.hpp"

namespace view {

struct view : public render_client {

    view();
    ~view();

    virtual void render() override;
    virtual bool close() override;
    virtual std::string window_name() override;


 private:
    void render_main_window();

    // this functional might be moved into another place in nearby future
    void init_variables();

    using rendering_client_ptr_t = std::unique_ptr<render_client>;
    std::unordered_map<std::string, rendering_client_ptr_t> clients_;

    model::datasource_ptr_t datasource_;

    // variables for imgui
    int symbol_box_id_;
    int period_box_id_;

    std::string symbol_content_;
};
} // namespace view