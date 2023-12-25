#include "Game/GFXCMDLogger.hpp"

#include <optional>

GFXCMDLogger& getGFXCMDLogger() {
    static auto inst = std::optional<GFXCMDLogger>();

    if (!inst.has_value()) {
        inst = GFXCMDLogger();
    }

    return inst.value();
}