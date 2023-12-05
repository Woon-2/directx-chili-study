#ifndef __Transform
#define __Transform

#include <DirectXMath.h>
#include "GFX/Core/GraphicsNamespaces.hpp"

#include <vector>
#include <memory>
#include <optional>
#include <ranges>
#include <algorithm>

class alignas(16) Transform {
public:
    Transform() noexcept
        : localMat_(dx::XMMatrixIdentity()) {}

    Transform(dx::FXMMATRIX mat) noexcept
        : localMat_(mat) {}

    void adjust(dx::FXMMATRIX mat) noexcept {
        localMat_ *= mat;
    }

    void set(dx::FXMMATRIX mat) noexcept {
        localMat_ = mat;
    }

    dx::XMMATRIX get() const noexcept {
        return localMat_;
    }

    dx::XMMATRIX* data() noexcept {
        return &localMat_;
    }

    const dx::XMMATRIX* data() const noexcept {
        return &localMat_;
    }

    Transform& operator*=(const Transform rhs) noexcept {
        localMat_ *= rhs.localMat_;
        return *this;
    }

    friend Transform operator*(Transform lhs, const Transform& rhs) noexcept {
        return lhs *= rhs;
    }

private:
    dx::XMMATRIX localMat_;
};

class BasicTransformComponent {
public:
    BasicTransformComponent() = default;
    BasicTransformComponent(const Transform initialLocal,
        const Transform& initialGlobal
    ) : local_(initialLocal), global_(initialGlobal) {}

    void adjustLocal(const Transform trans) noexcept {
        local_ *= trans;
    }

    void setLocal(const Transform trans) noexcept {
        local_ = trans;
    }

    const Transform local() const noexcept {
        return local_;
    }

    void adjustGlobal(const Transform trans) noexcept {
        global_ *= trans;
    }

    void setGlobal(const Transform trans) noexcept {
        global_ = trans;
    }

    const Transform global() const noexcept {
        return global_;
    }

    void setTotal(const Transform trans) noexcept {
        total_ = trans;
    }

    const Transform total() const noexcept {
        return total_.value();
    }

    void addChild(std::weak_ptr<const Transform> child) {
        children_.push_back( std::move(child) );
    }

    const auto& children() const noexcept {
        return children_;
    }

    void update() {
        auto expiredRange = std::ranges::remove_if( children_,
            [](const auto& wp) { return wp.expired(); }
        );

        children_.erase(expiredRange.begin(), expiredRange.end());
    }

private:
    Transform local_;
    Transform global_;
    std::optional<Transform> total_;
    std::vector< std::weak_ptr<const Transform> > children_;
};

#endif