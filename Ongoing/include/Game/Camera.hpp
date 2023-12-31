#ifndef __Camera
#define __Camera

#include "Transform.hpp"

#include <optional>
#include <memory>

struct CameraViewTransDesc {
    dx::XMFLOAT3 eye;
    dx::XMFLOAT3 at;
    dx::XMFLOAT3 up;
};

struct CameraProjTransDesc {
    float fovy;
    float aspect;
    float nearZ;
    float farZ;
};

struct CameraVisionDesc {
    CameraViewTransDesc viewTransDesc;
    CameraProjTransDesc projTransDesc;
};

class CameraVision {
public:
    friend class Camera;

    CameraVision();
    CameraVision(const CameraVisionDesc& cvDesc);

    const BasicTransformComponent& viewTransComp() const noexcept {
        return viewTrans_;
    }

    BasicTransformComponent& viewTransComp() noexcept {
        return viewTrans_;
    }

    const BasicTransformComponent& projTransComp() const noexcept {
        return projTrans_;
    }

    BasicTransformComponent& projTransComp() noexcept {
        return projTrans_;
    }

private:
    void updateView(const CameraViewTransDesc& vtd);
    void updateProj(const CameraProjTransDesc& ptd);

    const CameraViewTransDesc& vtDesc() const noexcept {
        return viewTransDesc_;
    }

    CameraViewTransDesc& vtDesc() noexcept {
        return viewTransDesc_;
    }

    const CameraProjTransDesc& ptDesc() const noexcept {
        return projTransDesc_;
    }

    CameraProjTransDesc& ptDesc() noexcept {
        return projTransDesc_;
    }

    BasicTransformComponent viewTrans_;
    BasicTransformComponent projTrans_;
    CameraViewTransDesc viewTransDesc_;
    CameraProjTransDesc projTransDesc_;
};

class Camera {
public:
    Camera()
        : pVision_( std::make_shared<CameraVision>() ) {}

    Camera(const CameraVisionDesc& cvDesc)
        : pVision_( std::make_shared<CameraVision>(cvDesc) ) {}

    void update() {

    }

    const std::shared_ptr<CameraVision> vision() noexcept {
        return pVision_;
    }

    const std::shared_ptr<const CameraVision> vision() const noexcept {
        return pVision_;
    }

    void setParams( std::optional<float> fovy,
        std::optional<float> aspect,
        std::optional<float> nearZ,
        std::optional<float> farZ
    );

    void VCALL setEye(dx::FXMVECTOR eyeVal) {
        const auto& oldAt = pVision_->vtDesc().at;
        const auto& oldUp = pVision_->vtDesc().up;
        auto newEye = dx::XMFLOAT3();
        dx::XMStoreFloat3(&newEye, eyeVal);

        pVision_->updateView(
            CameraViewTransDesc{
                .eye = newEye,
                .at = oldAt,
                .up = oldUp
            }
        );
    }

    dx::XMVECTOR eye() {
        auto stored = pVision_->vtDesc().eye;
        return dx::XMLoadFloat3(&stored);
    }

    void VCALL setAt(dx::FXMVECTOR atVal) {
        const auto& oldEye = pVision_->vtDesc().eye;
        const auto& oldUp = pVision_->vtDesc().up;
        auto newAt = dx::XMFLOAT3();
        dx::XMStoreFloat3(&newAt, atVal);

        pVision_->updateView(
            CameraViewTransDesc{
                .eye = oldEye,
                .at = newAt,
                .up = oldUp
            }
        );
    }

    dx::XMVECTOR at() {
        auto stored = pVision_->vtDesc().at;
        return dx::XMLoadFloat3(&stored);
    }

    void VCALL setUp(dx::FXMVECTOR upVal) {
        const auto& oldEye = pVision_->vtDesc().eye;
        const auto& oldAt = pVision_->vtDesc().at;
        auto newUp = dx::XMFLOAT3();
        dx::XMStoreFloat3(&newUp, upVal);

        pVision_->updateView(
            CameraViewTransDesc{
                .eye = oldEye,
                .at = oldAt,
                .up = newUp
            }
        );
    }

    dx::XMVECTOR up() {
        auto stored = pVision_->vtDesc().up;
        return dx::XMLoadFloat3(&stored);
    }

    void rotateX(float theta);
    void rotateY(float theta);
    void rotateZ(float theta);
    void VCALL rotateAxis(dx::FXMVECTOR axis, float theta);

private:
    std::shared_ptr<CameraVision> pVision_;
};

#endif  // __Camera