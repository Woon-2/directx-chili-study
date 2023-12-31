#include "Game/SimulationUI.hpp"

#include "imgui.h"

void SimulationUI::render() {
    if ( willShow_ && ImGui::Begin( "Simulation Speed", &willShow_ ) ) {
        ImGui::SliderFloat( "Speed Factor", &speedFactor_, 0.f, 4.f );
        ImGui::InputText("Butts", buffer_, 1024u);

        ImGui::End();
    }
}