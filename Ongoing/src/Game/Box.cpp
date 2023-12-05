#include "Game/Box.hpp"

void MouseInputComponent<Box>::receive(const Mouse::Event& ev) {
    if (ev.moved()) {
        curPos_ = pConverter_->convert( ev.pos() );
    }
}

void MouseInputComponent<Box>::update() {
    lastPos_ = curPos_;
}

void Entity<Box>::update(milliseconds elapsed) {
    tc_->update(elapsed);
    tc_->setTotal( dx::XMMatrixTranspose(
        (tc_->local() * tc_->global()).get()
    ) );
    dc_->update(tc_->total());
}

void Entity<Box>::ctDrawComponent(GFXFactory factory, GFXPipeline pipeline,
    Scene& scene, const ChiliWindow& wnd
) {
    dc_.reset( new DrawComponent<Box>(factory, pipeline, scene, wnd) );
}

void Entity<Box>::ctInputComponent( const MousePointConverter& converter,
    const AppMousePoint& initialPos
) {
    ic_.reset( new MouseInputComponent<Box>(converter, initialPos) );
}

void Entity<Box>::ctTransformComponent(
    Distribution& distRadius,
    Distribution& distCTP,  // chi, theta, phi
    Distribution& distDeltaCTP, // chi, theta, phi
    Distribution& distDeltaRTY   // roll, yaw, pitch
) {
    tc_.reset( new GTransformComponent(
        distRadius, distCTP, distDeltaCTP, distDeltaRTY
    ) );
}

Loader<Box> Entity<Box>::loader() const noexcept {
    return Loader<Box>(*this);
}