#include "Game/GFXCMDLogFileView.hpp"

#include <optional>

GFXCMDLogFileView& getGFXCMDLogFileView() {
    static auto inst = std::optional<GFXCMDLogFileView>();

    if (!inst.has_value()) {
        inst = GFXCMDLogFileView();
    }

    return inst.value();
}