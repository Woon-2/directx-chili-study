#ifndef __GTransformComponent
#define __GTransformComponent

#include "GFX/Core/Transform.hpp"

#include "Chrono.hpp"

#include <random>

using Distribution = std::normal_distribution<float>;
static std::random_device rd;
static std::mt19937 rng( rd() );

class GTransformComponent : public gfx::BasicTransformComponent {
public:
    GTransformComponent() = default;
    template <class DistributionT>
    GTransformComponent(Distribution& distRadius,
        DistributionT& distCTP,  // chi, theta, phi
        DistributionT& distDeltaCTP, // chi, theta, phi
        DistributionT& distDeltaRTY   // roll, yaw, pitch
    ) : total_(), radius_(distRadius(rng)), dRoll_(distDeltaRTY(rng)),
        dPitch_(distDeltaRTY(rng)), dYaw_(distDeltaRTY(rng)),
        dChi_(distDeltaCTP(rng)), dTheta_(distDeltaCTP(rng)),
        dPhi_(distDeltaCTP(rng)), roll_(0.f),
        pitch_(0.f), yaw_(0.f), chi_(distCTP(rng)),
        theta_(distCTP(rng)), phi_(distCTP(rng)) {}

    void update(milliseconds elapsed);

    void VCALL setTotal(gfx::Transform total) noexcept {
        total_ = total;
    }

    const gfx::Transform& total() const noexcept {
        return total_;
    }

private:
    gfx::Transform total_;

    float radius_;

    float dRoll_;
    float dPitch_;
    float dYaw_;
    float dChi_;
    float dTheta_;
    float dPhi_;

    float roll_;
    float pitch_;
    float yaw_;
    float chi_;
    float theta_;
    float phi_;
};

#endif  // __GTransformComponent