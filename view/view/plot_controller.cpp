#include "plot_controller.hpp"

#include <GL/GLU.h>
#include <core/SkImage.h>
#include <core/SkCanvas.h>
#include <core/SkSurface.h>

namespace view {

plot_controller::plot_controller(model_ptr_t model, coordinate_t width, coordinate_t height)
: model_(model)
, texture_id_(0)
, x_(0)
, y_(0)
, width_(width)
, height_(height)
, last_mouse_position_{}
, is_mouse_was_pressed_(false)
, zoom_multiplier_(1.0)
, show_(true) {
    assert(model_);
    model_->init(width_, height_, x_, y_);
	process_rendering();
}

plot_controller::~plot_controller() {}

void plot_controller::render() {
    
    if (ImGui::Begin(model_->name().c_str(), &show_)) {



        ImVec2 dimensions = { static_cast<float>(width_), static_cast<float>(height_) };
        ImGui::Image((void*)texture_id_, dimensions);

        // process move
        int f = static_cast<int>(x_);
        ImGui::DragInt(model_->name().c_str(), &f, -1);
        if (static_cast<unsigned>(f) != x_) {
            x_ = (f < 0 ? 0 : f);
            //y_ -= (move.y > y_ ? y_ : move.y);
            process_rendering();
        } 

        // separate from another
        if (check_process_mouse_wheel()) {
            process_rendering();
        }
    }
    ImGui::End();
}

bool plot_controller::close() {
    return !show_;
}

std::string plot_controller::window_name()
{
    return model_->name();;
}

void plot_controller::resize(coordinate_t width, coordinate_t height){
}

void plot_controller::process_rendering(){

    if (texture_id_ != 0) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }

	auto image_data = render_image_in_skia();

    // Write data to OpenGL texture
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE,
                 image_data.data());
}

//ImVec2 plot_controller::compute_image_move()
//{
//    // ImVec2 result{0,0};
//
//    //ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
//    //ImVec2 mouse_position = ImGui::GetMousePos();
//
//    float f = x_;
//    ImGui::DragFloat(model_->name().c_str(), &f, -1);
//    
// //   ImVec2 relative_position(mouse_position.x - tex_screen_pos.x, -mouse_position.y + tex_screen_pos.y);
// //   bool   is_mouse_inside = relative_position.x > 0 && relative_position.y > 0 &&
// //                          relative_position.x < static_cast<float>(width_) &&
// //                          relative_position.y < static_cast<float>(height_);
// //   bool is_mouse_pressed = ImGui::IsMouseDown(0);
//
//	//// if mouse changes state do nothing
//	//if ((is_mouse_pressed && !is_mouse_was_pressed_) ||
//	//	(!is_mouse_pressed && is_mouse_was_pressed_)) {
//	//	is_mouse_was_pressed_ = !is_mouse_was_pressed_;
//	//	last_mouse_position_ = relative_position;
//	//	return result;
//	//}
//
//	//// state does not changed
//	//if (!is_mouse_was_pressed_) {
//	//	return result;
//	//}
//	//
//
//
//	//// mouse was held
//	//result.x = relative_position.x - last_mouse_position_.x;
//	//result.y = relative_position.y - last_mouse_position_.y;
//	//last_mouse_position_ = relative_position;
//
//	return result;
//}

bool plot_controller::check_process_mouse_wheel()
{
    bool result = false;
    if (ImGui::GetIO().MouseWheel > 0) {
        zoom_multiplier_ -= 0.05;
        result = true;
    }
    else if (ImGui::GetIO().MouseWheel < 0) {
        zoom_multiplier_ += 0.05;
        result = true;
    }
    if (result) {
        model_->zoom(zoom_multiplier_, width_, height_, x_, y_);
    }
    return result;
}

std::vector<unsigned char> plot_controller::render_image_in_skia() {

	std::vector<unsigned char> image_data(width_ * height_ * 4);
	memset(image_data.data(), 0, width_ * height_ * 4);

	SkImageInfo canvas_info = SkImageInfo::MakeN32(width_, height_, kPremul_SkAlphaType);
	SkPixmap    dst_pixmap(canvas_info, (void*)image_data.data(), width_ * 4);

    // generating image in skia
    sk_sp<SkSurface> raster_surface = SkSurface::MakeRasterN32Premul(width_, height_);
    SkCanvas*        raster_canvas  = raster_surface->getCanvas();

    model_->draw(raster_canvas, width_, height_, x_, y_);

    sk_sp<SkImage> img(raster_surface->makeImageSnapshot());
    if (!img) {
        throw std::runtime_error("Cannot create an image");
    }

	img->readPixels(dst_pixmap, 0, 0);

    return image_data;
}


}