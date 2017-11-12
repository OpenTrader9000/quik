#pragma once

#include "controller/controller.hpp"
#include "model/model.hpp"

#include <imgui/imgui.h>
#include <imgui/opengl/gl3w/GL/glcorearb.h>

#include <core/SkImage.h>

namespace view {
// view + controller
struct imgui {

    using coordinate_t = model::model::coordinate_t;

    imgui(model_ptr_t model, coordinate_t width = 0, coordinate_t height = 0);
    ~imgui();

    void render();
    void resize(coordinate_t width, coordinate_t height);

 private:
	 // all OpenGL functions are maked here
    void process_rendering();
	ImVec2 compute_image_move() ;

	std::vector<unsigned char>	render_image_in_skia();

    model_ptr_t model_;
    GLuint      texture_id_;

	coordinate_t x_;
	coordinate_t y_;
    coordinate_t width_;
    coordinate_t height_;

    // mouse state
    ImVec2 last_mouse_position_;
	bool   is_mouse_was_pressed_;
};
} // namespace view