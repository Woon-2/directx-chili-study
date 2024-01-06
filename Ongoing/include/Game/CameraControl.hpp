#ifndef __CameraControl
#define __CameraControl

#include "Camera.hpp"

class CoordSystem;

class CameraControl {
public:
    CameraControl()
        : r_(20.f), theta_(0.f), phi_(0.f),
        chi_(0.f), roll_(0.f), pitch_(0.f),
        yaw_(0.f), willShow_(false) {}

    void render();
    void reflect(CoordSystem& cameraCoord);

    bool shown() const noexcept {
        return willShow_;
    }

    void show() noexcept {
        willShow_ = true;
    }

    void close() noexcept {
        willShow_ = false;
    }

private:
    float r_;
    float theta_;
    float phi_;
    float chi_;
    float roll_;
    float pitch_;
    float yaw_;
    bool willShow_;
};

#endif  // __CameraControl