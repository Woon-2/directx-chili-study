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
            .nearZ = 0.f,
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

    viewTrans_.setLocal( dx::XMMatrixLookAtLH(eye, at, up) );
    projTrans_.setLocal( dx::XMMatrixPerspectiveFovLH(fovy, aspect, nearZ, farZ) );
}

void CameraVision::updateView(const CameraViewTransDesc& vtd) {
    auto eye = dx::XMLoadFloat3(&vtd.eye);
    auto at = dx::XMLoadFloat3(&vtd.at);
    auto up = dx::XMLoadFloat3(&vtd.up);

    viewTrans_.setLocal( dx::XMMatrixLookAtLH(eye, at, up) );

    viewTransDesc_ = vtd;
}

void CameraVision::updateProj(const CameraProjTransDesc& ptd) {
    auto& fovy = ptd.fovy;
    auto& aspect = ptd.aspect;
    auto& nearZ = ptd.nearZ;
    auto& farZ = ptd.farZ;

    projTrans_.setLocal( dx::XMMatrixPerspectiveFovLH(fovy, aspect, nearZ, farZ) );

    projTransDesc_ = ptd;
}

void Camera::setParams( std::optional<float> fovy,
    std::optional<float> aspect,
    std::optional<float> nearZ,
    std::optional<float> farZ
) {
    vision_.updateProj(
        CameraProjTransDesc{
            .fovy = fovy.value_or( vision_.ptDesc().fovy ),
            .aspect = aspect.value_or( vision_.ptDesc().aspect ),
            .nearZ = nearZ.value_or( vision_.ptDesc().nearZ ),
            .farZ = farZ.value_or( vision_.ptDesc().farZ )
        }
    );
}

inline namespace {
    template <class RotateFn>
    dx::XMMATRIX rotateImpl( dx::XMMATRIX mat, dx::XMVECTOR axis,
        float theta, RotateFn&& rfn
    ) {
        // store translation
        auto eye = mat.r[3];
        // invert translation
        // (DXMath is using row-major matrices, so 4th row represents the translation)
        mat.r[3] = dx::XMVectorZero();
        mat *= std::invoke( std::forward<RotateFn>(rfn), axis, theta );
        // translate with stored translation
        return mat * dx::XMMatrixTranslationFromVector(eye);
    }
}

void Camera::rotateX(float theta) {
    auto& vtMat = *( vision_.viewTransComp().localRef().data() );

    vtMat *= rotateImpl( vtMat, {}, theta,
        [](auto _, auto theta) {
            return dx::XMMatrixRotationX(theta);
        }
    );
}

void Camera::rotateY(float theta) {
    auto& vtMat = *( vision_.viewTransComp().localRef().data() );

    vtMat *= rotateImpl( vtMat, {}, theta,
        [](auto _, auto theta) {
            return dx::XMMatrixRotationY(theta);
        }
    );
}

void Camera::rotateZ(float theta) {
    auto& vtMat = *( vision_.viewTransComp().localRef().data() );

    vtMat *= rotateImpl( vtMat, {}, theta,
        [](auto _, auto theta) {
            return dx::XMMatrixRotationZ(theta);
        }
    );
}

void Camera::rotateAxis(dx::XMVECTOR axis, float theta) {
    auto& vtMat = *( vision_.viewTransComp().localRef().data() );

    vtMat *= rotateImpl( vtMat, {}, theta,
        [](auto axis, auto theta) {
            return dx::XMMatrixRotationAxis(axis, theta);
        }
    );
}