#include "Game/GTransformComponent.hpp"

void GTransformComponent::update(milliseconds elapsed) {
    BasicTransformComponent::update();

    float elapsedCnt = std::chrono::duration_cast<seconds>(elapsed).count();

    roll_ += dRoll_ * elapsedCnt;
    pitch_ += dPitch_ * elapsedCnt;
    yaw_ += dYaw_ * elapsedCnt;
    chi_ += dChi_ * elapsedCnt;
    theta_ += dRoll_ * elapsedCnt;
    phi_ += dPhi_ * elapsedCnt;

    setLocal(
        dx::XMMatrixRotationRollPitchYaw( pitch_, yaw_, roll_ )
    );

    setGlobal(
        dx::XMMatrixTranslation( radius_, 0.f, 0.f )
        * dx::XMMatrixRotationRollPitchYaw( theta_, phi_, chi_ )
        * dx::XMMatrixTranslation( 0.f, 0.f, 20.f )
        * dx::XMMatrixPerspectiveLH( 1.f, 3.f/4.f, 0.5f, 40.f )
    );
}