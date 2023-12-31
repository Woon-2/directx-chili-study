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

    void rotateX(float theta);
    void rotateY(float theta);
    void rotateZ(float theta);
    void VCALL rotateAxis(dx::FXMVECTOR axis, float theta);

private:
    std::shared_ptr<CameraVision> pVision_;
};

#endif  // __Camera