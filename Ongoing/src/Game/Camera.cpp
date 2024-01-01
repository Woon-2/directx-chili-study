#include "Game/Camera.hpp"

#include <functional>

CameraVision::CameraVision()
    : CameraVision( CameraVisionDesc {
        .viewTransDesc = CameraViewTransDesc{
            .eye = dx::XMFLOAT3(0.f, 0.f, 0.f),
            .at = dx::XMFLOAT3(0.f, 0.f ,1.f),
            .up = dx::XMFLOAT3(0.f, 1.f, 0.f)
        },
        .projTransDesc = CameraProjTransDesc{
            .fovy = dx::XM_PIDIV2,
            .aspect = 1.f,
            .nearZ = 0.5f,
            .farZ = 40.f
        }
    } ) {}

CameraVision::CameraVision(const CameraVisionDesc& cvDesc)
    : viewTrans_(), projTrans_(),
    viewTransDesc_(cvDesc.viewTransDesc),
    projTransDesc_(cvDesc.projTransDesc) {
    auto eye = dx::XMLoadFloat3(&cvDesc.viewTransDesc.eye);
    auto at = dx::XMLoadFloat3(&cvDesc.viewTransDesc.at);
    auto up = dx::XMLoadFloat3(&cvDesc.viewTransDesc.up);

    auto& fovy = cvDesc.projTransDesc.fovy;
    auto& aspect = cvDesc.projTransDesc.aspect;
    auto& nearZ = cvDesc.projTransDesc.nearZ;
    auto& farZ = cvDesc.projTransDesc.farZ;

    viewTrans_ = dx::XMMatrixLookAtLH(eye, at, up);
    projTrans_ = dx::XMMatrixPerspectiveFovLH(fovy, aspect, nearZ, farZ);
}

void CameraVision::updateView(const CameraViewTransDesc& vtd) {
    auto eye = dx::XMLoadFloat3(&vtd.eye);
    auto at = dx::XMLoadFloat3(&vtd.at);
    auto up = dx::XMLoadFloat3(&vtd.up);

    viewTrans_ = dx::XMMatrixLookAtLH(eye, at, up);

    viewTransDesc_ = vtd;
}

void CameraVision::updateProj(const CameraProjTransDesc& ptd) {
    auto& fovy = ptd.fovy;
    auto& aspect = ptd.aspect;
    auto& nearZ = ptd.nearZ;
    auto& farZ = ptd.farZ;

    projTrans_ = dx::XMMatrixPerspectiveFovLH(fovy, aspect, nearZ, farZ);

    projTransDesc_ = ptd;
}

void Camera::setParams( std::optional<float> fovy,
    std::optional<float> aspect,
    std::optional<float> nearZ,
    std::optional<float> farZ
) {
    pVision_->updateProj(
        CameraProjTransDesc{
            .fovy = fovy.value_or( pVision_->ptDesc().fovy ),
            .aspect = aspect.value_or( pVision_->ptDesc().aspect ),
            .nearZ = nearZ.value_or( pVision_->ptDesc().nearZ ),
            .farZ = farZ.value_or( pVision_->ptDesc().farZ )
        }
    );
}

void Camera::rotateX(float theta) {
    pVision_->viewTrans() *= dx::XMMatrixRotationX(theta);
}

void Camera::rotateY(float theta) {
    pVision_->viewTrans() *= dx::XMMatrixRotationY(theta);
}

void Camera::rotateZ(float theta) {
    pVision_->viewTrans() *= dx::XMMatrixRotationZ(theta);
}

void VCALL Camera::rotateAxis(dx::FXMVECTOR axis, float theta) {
    pVision_->viewTrans() *= dx::XMMatrixRotationAxis(axis, theta);
}