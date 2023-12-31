#include "Game/SimulationUI.hpp"

#include "imgui.h"

void SimulationUI::render() {
    if ( willShow_ && ImGui::Begin( "Simulation Speed", &willShow_ ) ) {
        ImGui::SliderFloat( "Speed Factor", &speedFactor_, 0.f, 4.f );
        ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)",
            1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate
        );
        ImGui::InputText("Butts", buffer_, 1024u);

        ImGui::End();
    }
}