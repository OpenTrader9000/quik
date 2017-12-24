#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

#include <imgui/opengl/run.hpp>
#include <imgui/imgui.h>

#include <core/SkImage.h>
#include <core/SkCanvas.h>
#include <core/SkRRect.h>
#include <core/SkPath.h>
#include <core/SkData.h>
#include <core/SkImage.h>
#include <core/SkStream.h>
#include <core/SkSurface.h>

#include <view/model/trading.hpp>
//#include <view/imgui.hpp>
#include <view/model/persistent_datasource.hpp>
#include "view/render.hpp"

#include "ohlc.hpp"

float g_width = 1024;
float g_height = 480;


//view::plot_controller global_view(std::make_shared<view::model::trading>(), g_width, g_height);

bool show_window = true;
bool show_image  = true;
void render(void* device) {

    // ImGui::BeginChild("Image window", ImVec2{ g_width + 20, g_height + 20 }, true,
    //                  ImGuiWindowFlags_HorizontalScrollbar);
    view::main_window()->render();
    // ImGui::EndChild();
}


int main(int argc, char* argv[]) {

    view::model::make_persistent_datasource(R"(D:\archive2\2017)");
    view::init_render();

	//raster(256, 256, draw, "out.png");
	imgui_run(render);
}
