#include "GFX/Scenery/Camera.hpp"

#include <functional>

namespace gfx {
namespace scenery {

CameraVision::CameraVision()
    : CameraVision( CameraVisionDesc{
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

const CameraViewTransDesc
Camera::CameraCoordComponent::makeVTDesc() const noexcept {
    auto worldTrans = coordSystem_.total();
    
    auto eye = dx::XMVector4Transform(
        dx::XMVectorSet(0.f, 0.f, 0.f, 1.f),
        worldTrans.get()
    );
    auto at = dx::XMVector4Transform(
        dx::XMVectorSet(0.f, 0.f, 1.f, 1.f),
        worldTrans.get()
    );
    auto up = dx::XMVector4Transform(
        dx::XMVectorSet(0.f, 1.f, 0.f, 0.f),
        worldTrans.get()
    );

    auto ret = CameraViewTransDesc{};
    dx::XMStoreFloat3(&ret.eye, eye);
    dx::XMStoreFloat3(&ret.at, at);
    dx::XMStoreFloat3(&ret.up, up);

    return ret;
}

void Camera::setParams( std::optional<float> fovy,
    std::optional<float> aspect,
    std::optional<float> nearZ,
    std::optional<float> farZ
) {
    vision().updateProj(
        CameraProjTransDesc{
            .fovy = fovy.value_or( vision().ptDesc().fovy ),
            .aspect = aspect.value_or( vision().ptDesc().aspect ),
            .nearZ = nearZ.value_or( vision().ptDesc().nearZ ),
            .farZ = farZ.value_or( vision().ptDesc().farZ )
        }
    );
}

void Camera::rotateX(float theta) {
    vision().viewTrans() *= dx::XMMatrixRotationX(theta);
}

void Camera::rotateY(float theta) {
    vision().viewTrans() *= dx::XMMatrixRotationY(theta);
}

void Camera::rotateZ(float theta) {
    vision().viewTrans() *= dx::XMMatrixRotationZ(theta);
}

void VCALL Camera::rotateAxis(dx::FXMVECTOR axis, float theta) {
    vision().viewTrans() *= dx::XMMatrixRotationAxis(axis, theta);
}

} // namespace gfx::scenery
} // namespace gfx