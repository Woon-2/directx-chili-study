#include "GFX/Core/Storage.hpp"

namespace gfx {

std::optional<GFXRes::Dummy> GFXRes::nullTag;

GFXRes::GFXRes(GFXRes&& other) noexcept
    : gen_(std::move(other.gen_)),
    stored_(std::move(other.stored_)),
    genMode_(other.genMode_) {
    // https://en.cppreference.com/w/cpp/utility/optional/operator%3D
    // moved-from optional doesn't be reset.
    other.stored_.reset();
}

GFXRes& GFXRes::operator=(GFXRes&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    destroy();

    gen_ = std::move(other.gen_);
    stored_ = std::move(other.stored_);
    // https://en.cppreference.com/w/cpp/utility/optional/operator%3D
    // moved-from optional doesn't be reset.
    other.stored_.reset();
    genMode_ = other.genMode_;

    return *this;
}

}   // namespace gfx