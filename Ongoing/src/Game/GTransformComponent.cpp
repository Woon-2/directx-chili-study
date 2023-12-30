#include "Game/GTransformComponent.hpp"

void GTransformComponent::update(milliseconds elapsed) {
    BasicTransformComponent::update();

    float elapsedCnt = std::chrono::duration_cast<seconds>(elapsed).count();

    roll_ += dRoll_ * elapsedCnt;
    pitch_ += dPitch_ * elapsedCnt;
    yaw_ += dYaw_ * elapsedCnt;
    chi_ += dChi_ * elapsedCnt;
    theta_ += dTheta_ * elapsedCnt;
    phi_ += dPhi_ * elapsedCnt;

    setLocal(
        dx::XMMatrixRotationRollPitchYaw( pitch_, yaw_, roll_ )
    );

    setGlobal(
        dx::XMMatrixTranslation( radius_, 0.f, 0.f )
        * dx::XMMatrixRotationRollPitchYaw( theta_, phi_, chi_ )
    );
}