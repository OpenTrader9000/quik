#pragma once

#include <functional>

int imgui_run(std::function<void(void*)> render_f);