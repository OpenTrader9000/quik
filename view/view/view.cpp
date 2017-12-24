#include "view.hpp"

#include "model/trading.hpp"
#include "plot_controller.hpp"

#include <imgui/imgui.h>
#include <utils/string/string.hpp>
#include <persistent/trade/data.hpp>

namespace view {

view::view()
    : datasource_(model::get_datasource()) {
    init_variables();
}

view::~view() {}

void view::render() {
    render_main_window();
    for (auto& cli : clients_)
        cli.second->render();
}

bool view::close() {
    return false;
}

char const* win_name = "Main window";
std::string view::window_name() {
    return win_name;
}

void view::render_main_window() {
    bool show_window = true;
    if (ImGui::Begin(win_name, &show_window)) {

        char const* periods_str[] = { "min", "5 min", "10 min", "15 min", "30 min", "hour", "day", "week" };
        ImGui::Combo("period", &period_box_id_, periods_str, ((int)(sizeof(periods_str) / sizeof(*periods_str))));
        ImGui::Combo("symbol", &symbol_box_id_, symbol_content_.c_str());
        if (ImGui::Button("Create") && symbol_box_id_ != -1 && period_box_id_ != -1) {

            using namespace persistent::trade;

            period periods[] = { p1min,p5min,p10min,p15min,p30min,p60min,pday,pweek };

            period             per = periods[period_box_id_];
            std::string const& symbol = datasource_->symbols()[symbol_box_id_];

            //window per symbol
            if (clients_.find(symbol) == clients_.end()) {
                auto model = std::make_shared<model::trading>(symbol, per);
                clients_.emplace(std::make_pair(symbol, std::make_unique<plot_controller>(model, 1024, 540)));
            }
        }

    }
    ImGui::End();
}

void view::init_variables() {
    symbol_box_id_ = 2;
    period_box_id_ = 2;

    // workaround for ImGui::ComboBox
    auto symbols = datasource_->symbols();
    for (auto const& sym : symbols) {
        symbol_content_ += sym;
        symbol_content_ += '\0';
    }
    symbol_content_ += '\0';

}

} // namespace view