#ifndef __CameraControl
#define __CameraControl

#include "GFX/Scenery/Camera.hpp"

class CoordSystem;

class CameraControl {
public:
    CameraControl() noexcept;

    void render();
    void submit(CoordSystem& cameraCoord);

    bool shown() const noexcept {
        return willShow_;
    }

    void show() noexcept {
        willShow_ = true;
    }

    void close() noexcept {
        willShow_ = false;
    }

    void reset() noexcept;

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