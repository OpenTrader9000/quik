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
#include <view/imgui.hpp>

float g_width = 1024;
float g_height = 480;

view::imgui global_view(std::make_shared<view::model::trading>(), g_width, g_height);

bool show_window = true;
bool show_image  = true;
void render(void* device) {

    if (ImGui::Begin("Image window", &show_window)) {

        ImGui::BeginChild("Image window", ImVec2{ g_width + 20, g_height + 20 }, true,
                          ImGuiWindowFlags_HorizontalScrollbar);
        global_view.render();
        ImGui::EndChild();

        // ImGui::Text(
        //"Application average %.3f ms/frame (%.1f FPS), isHovered: %d, leftMouseDown: %d, "
        //"position: "
        //"{%.1f, %.1f}, wheel: %.f",
        // 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate, false, false, 0, 0,
        // ImGui::GetIO().MouseWheel);
    }
    ImGui::End();
}


int main(int argc, char* argv[]) {
	//raster(256, 256, draw, "out.png");
	imgui_run(render);
}