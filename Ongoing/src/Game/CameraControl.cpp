#include "Game/CameraControl.hpp"

#include "Game/CoordSystem.hpp"

#include "imgui.h"

CameraControl::CameraControl() noexcept
    : r_(20.f), theta_(0.f), phi_(0.f),
    chi_(0.f), roll_(0.f), pitch_(0.f),
    yaw_(0.f), willShow_(false) {}

void CameraControl::render() {
    if ( !shown() ) {
        return;
    }

    if ( ImGui::Begin( "Camera Control", &willShow_ ) ) {
        ImGui::Text("Position");
        ImGui::SliderFloat( "R", &r_, 0.f, 80.f, "%.1f" );
        ImGui::SliderAngle( "Theta", &theta_, -180.f, 180.f );
        ImGui::SliderAngle( "Phi", &phi_, -89.f, 89.f );
        ImGui::SliderAngle( "Chi", &chi_, -180.f, 180.f );

        ImGui::Text("Orientation");
        ImGui::SliderAngle( "Roll", &roll_, -180.f, 180.f );
        ImGui::SliderAngle( "Pitch", &pitch_, -89.f, 89.f );
        ImGui::SliderAngle( "Yaw", &yaw_, -180.f, 180.f );
        
        if ( ImGui::Button( "Reset", ImVec2(40.f, 30.f) ) ) {
            reset();
        }

        ImGui::End();
    }
}

void CameraControl::submit(CoordSystem& cameraCoord) {
    cameraCoord.setLocal(
        dx::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_)
    );

    cameraCoord.setGlobal(
        dx::XMMatrixTranslation(0.f, 0.f, -r_)
        * dx::XMMatrixRotationRollPitchYaw(phi_, chi_, theta_)
    );
}

void CameraControl::reset() noexcept {
    r_ = 20.f;
    theta_ = 0.f;
    phi_ = 0.f;
    chi_ = 0.f;
    roll_ = 0.f;
    pitch_ = 0.f;
    yaw_ = 0.f;
}