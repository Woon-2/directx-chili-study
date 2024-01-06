#ifndef __Transform
#define __Transform

#include "GFX/Core/GraphicsNamespaces.hpp"

#include <vector>
#include <memory>
#include <optional>

class alignas(16) Transform {
public:
    Transform() noexcept
        : localMat_(dx::XMMatrixIdentity()) {}

    Transform(dx::FXMMATRIX mat) noexcept
        : localMat_(mat) {}

    void VCALL adjust(dx::FXMMATRIX mat) noexcept {
        localMat_ *= mat;
    }

    void VCALL set(dx::FXMMATRIX mat) noexcept {
        localMat_ = mat;
    }

    dx::XMMATRIX clone() const noexcept {
        return localMat_;
    }

    dx::XMMATRIX& get() noexcept {
        return localMat_;
    }

    const dx::XMMATRIX& get() const noexcept {
        return localMat_;
    }

    dx::XMMATRIX* data() noexcept {
        return &localMat_;
    }

    const dx::XMMATRIX* data() const noexcept {
        return &localMat_;
    }

    Transform& VCALL operator*=(const Transform rhs) noexcept {
        localMat_ *= rhs.localMat_;
        return *this;
    }

    friend Transform VCALL operator*(Transform lhs, const Transform& rhs) noexcept {
        return lhs *= rhs;
    }

    Transform VCALL transpose() const noexcept {
        return dx::XMMatrixTranspose(localMat_);
    }

private:
    dx::XMMATRIX localMat_;
};

class BasicTransformComponent {
public:
    BasicTransformComponent()
        : local_(), global_() {}

    BasicTransformComponent(const Transform& initialLocal,
        const Transform& initialGlobal
    ) : local_(initialLocal), global_(initialGlobal) {}

    void VCALL adjustLocal(const Transform trans) noexcept {
        local_ *= trans;
    }

    void VCALL setLocal(const Transform trans) noexcept {
        local_ = trans;
    }

    Transform& local() noexcept {
        return local_;
    }

    const Transform& local() const noexcept {
        return local_;
    }

    void VCALL adjustGlobal(const Transform trans) noexcept {
        global_ *= trans;
    }

    void VCALL setGlobal(const Transform trans) noexcept {
        global_ = trans;
    }

    Transform& global() noexcept {
        return global_;
    }

    const Transform& global() const noexcept {
        return global_;
    }

private:
    Transform local_;
    Transform global_;
};

#endif