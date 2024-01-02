#ifndef __Camera
#define __Camera

#include "Transform.hpp"
#include "CoordSystem.hpp"

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

    auto& viewTrans() noexcept {
        return viewTrans_;
    }

    const auto& viewTrans() const noexcept {
        return viewTrans_;
    }

    const auto& projTrans() const noexcept {
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

    Transform viewTrans_;
    Transform projTrans_;
    CameraViewTransDesc viewTransDesc_;
    CameraProjTransDesc projTransDesc_;
};



class Camera {
private:
    class CameraCoordComponent {
    public:
        CameraCoordComponent()
            : coordSystem_() {}

        const CameraViewTransDesc makeVTDesc() const noexcept {
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

        CoordSystem& coordSystem() noexcept {
            return coordSystem_;
        }

        const CoordSystem& coordSystem() const noexcept {
            return coordSystem_;
        }

    private:
        CoordSystem coordSystem_;
    };

public:
    Camera()
        : vision_(), coordComp_() {}

    Camera(const CameraVisionDesc& cvDesc)
        : vision_(cvDesc), coordComp_() {}

    void update() {
        if (coordComp_.has_value()) {
            vision().updateView( coordComp_.value().makeVTDesc() );
        }
    }

    void attach(const CoordSystem& targetCoord) {
        detach();
        coordComp_ = CameraCoordComponent();
        coordComp_.value().coordSystem().setParent(targetCoord);
        targetCoord.addChild(coordComp_.value().coordSystem());
    }

    void detach() {
        coordComp_.reset();
    }

    CoordSystem& coordSystem() {
        if (!coordComp_.has_value()) {
            // do proper error handling
            throw;
        }
        return coordComp_.value().coordSystem();
    }

    const CoordSystem& coordSystem() const {
        if (!coordComp_.has_value()) {
            // do proper error handling
            throw;
        }
        return coordComp_.value().coordSystem();
    }

    CameraVision& vision() noexcept {
        return vision_;
    }

    const CameraVision& vision() const noexcept {
        return vision_;
    }

    void setParams( std::optional<float> fovy,
        std::optional<float> aspect,
        std::optional<float> nearZ,
        std::optional<float> farZ
    );

    void VCALL setEye(dx::FXMVECTOR eyeVal) {
        const auto& oldAt = vision().vtDesc().at;
        const auto& oldUp = vision().vtDesc().up;
        auto newEye = dx::XMFLOAT3();
        dx::XMStoreFloat3(&newEye, eyeVal);

        vision().updateView(
            CameraViewTransDesc{
                .eye = newEye,
                .at = oldAt,
                .up = oldUp
            }
        );
    }

    dx::XMVECTOR eye() {
        auto stored = vision().vtDesc().eye;
        return dx::XMLoadFloat3(&stored);
    }

    void VCALL setAt(dx::FXMVECTOR atVal) {
        const auto& oldEye = vision().vtDesc().eye;
        const auto& oldUp = vision().vtDesc().up;
        auto newAt = dx::XMFLOAT3();
        dx::XMStoreFloat3(&newAt, atVal);

        vision().updateView(
            CameraViewTransDesc{
                .eye = oldEye,
                .at = newAt,
                .up = oldUp
            }
        );
    }

    dx::XMVECTOR at() {
        auto stored = vision().vtDesc().at;
        return dx::XMLoadFloat3(&stored);
    }

    void VCALL setUp(dx::FXMVECTOR upVal) {
        const auto& oldEye = vision().vtDesc().eye;
        const auto& oldAt = vision().vtDesc().at;
        auto newUp = dx::XMFLOAT3();
        dx::XMStoreFloat3(&newUp, upVal);

        vision().updateView(
            CameraViewTransDesc{
                .eye = oldEye,
                .at = oldAt,
                .up = newUp
            }
        );
    }

    dx::XMVECTOR up() {
        auto stored = vision().vtDesc().up;
        return dx::XMLoadFloat3(&stored);
    }

    void rotateX(float theta);
    void rotateY(float theta);
    void rotateZ(float theta);
    void VCALL rotateAxis(dx::FXMVECTOR axis, float theta);

private:
    CameraVision vision_;
    std::optional<CameraCoordComponent> coordComp_;
};

#endif  // __Camera