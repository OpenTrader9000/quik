#pragma once
#include <string>
#include <memory>

namespace view {
    struct render_client {
        virtual ~render_client() {}
        virtual void        render()      = 0;
        virtual bool        close()       = 0;
        virtual std::string window_name() = 0;
    };

    std::shared_ptr<render_client> main_window();
    void init_render();
}