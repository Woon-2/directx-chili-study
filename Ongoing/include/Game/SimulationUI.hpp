#ifndef __SimulationUI
#define __SimulationUI

class SimulationUI {
public:
    SimulationUI()
        : speedFactor_(1.f), buffer_{0} {}

    void render();

    float speedFactor() const noexcept {
        return speedFactor_;
    }
private:
    float speedFactor_;

    char buffer_[1024];  // temporary
};

#endif  // __SimulationUI