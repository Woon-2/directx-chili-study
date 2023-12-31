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
        : local_(), global_(), total_(Transform()) {}
        
    BasicTransformComponent(const Transform& initialLocal,
        const Transform& initialGlobal
    ) : local_(initialLocal), global_(initialGlobal),
    total_(Transform()) {}

    void VCALL adjustLocal(const Transform trans) noexcept {
        local_ *= trans;
    }

    void VCALL setLocal(const Transform trans) noexcept {
        local_ = trans;
    }

    const Transform local() const noexcept {
        return local_;
    }

    Transform& localRef() noexcept {
        return local_;
    }

    const Transform& localRef() const noexcept {
        return local_;
    }

    void VCALL adjustGlobal(const Transform trans) noexcept {
        global_ *= trans;
    }

    void VCALL setGlobal(const Transform trans) noexcept {
        global_ = trans;
    }

    const Transform global() const noexcept {
        return global_;
    }

    Transform& globalRef() noexcept {
        return global_;
    }

    const Transform& globalRef() const noexcept {
        return global_;
    }

    void VCALL setTotal(const Transform trans) noexcept {
        total_ = trans;
    }

    const Transform total() const noexcept {
        return total_.value();
    }

private:
    Transform local_;
    Transform global_;
    std::optional<Transform> total_;
};

#endif