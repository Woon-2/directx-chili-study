#ifndef __CoordSystem
#define __CoordSystem

#include "Transform.hpp"

#include <memory>
#include <unordered_map>
#include <optional>
#include <functional>
#include <ranges>
#include <algorithm>
#include <vector>
#include <cassert>

#include "GFX/Core/Namespaces.hpp"

class CoordSystemID {
public:
    CoordSystemID()
        : val_(gen().val_) {}

    CoordSystemID(std::size_t val)
        : val_(val) {}

    struct Hash {
        std::size_t operator()(CoordSystemID key) const noexcept {
            return std::hash<std::size_t>{}(key.val_);
        }

        std::size_t operator()(
            std::reference_wrapper<CoordSystemID> key
        ) const noexcept {
            return operator()(key.get());
        }
    };

    friend auto operator<=>(
        CoordSystemID lhs, CoordSystemID rhs
    ) noexcept = default;

    static CoordSystemID gen();

private:
    std::size_t val_;
};

class CoordSystem {
public:
    CoordSystem()
        : tc_(), total_( Transform() ), parent_(),
        children_(), id_() {}

    ~CoordSystem();
    CoordSystem(const CoordSystem&) = default;
    CoordSystem& operator=(const CoordSystem&) = default;
    CoordSystem(CoordSystem&&) noexcept = default;
    CoordSystem& operator=(CoordSystem&&) noexcept = default;

    void addChild(CoordSystem& child) const {
        assert( std::ranges::find(children_, &child) == children_.end() );
        linkParentChild(*this, child);
    }

    void setParent(const CoordSystem& parent) {
        if (parent_.has_value() && parent_.value() == &parent) [[unlikely]] {
            return;
        }
        linkParentChild(parent, *this);
    }

    void loseParent();
    bool lostParent() const noexcept {
        return !parent_.has_value();
    }
    void traverse();

    const CoordSystemID id() const noexcept {
        return id_;
    }

    void VCALL adjustLocal(const Transform trans) noexcept {
        tc_.adjustLocal(trans);
        setDirty();
    }

    void VCALL setLocal(const Transform trans) noexcept {
        tc_.setLocal(trans);
        setDirty();
    }

    const Transform& local() const noexcept {
        return tc_.local();
    }

    void VCALL adjustGlobal(const Transform trans) noexcept {
        tc_.adjustGlobal(trans);
        setDirty();
    }

    void VCALL setGlobal(const Transform trans) noexcept {
        tc_.setGlobal(trans);
        setDirty();
    }

    const Transform& global() const noexcept {
        return tc_.global();
    }

    void setDirty() noexcept {
        total_.reset();
    }

    bool dirty() const noexcept {
        return !total_.has_value();
    }

    void destroyCascade() noexcept;

    const Transform& total() const noexcept {
        assert(!dirty());
        return total_.value();
    }

    friend auto operator<=>( const CoordSystem& lhs,
         const CoordSystem& rhs
    ) {
        return lhs.id() <=> rhs.id();
    }

    friend auto operator==( const CoordSystem& lhs,
         const CoordSystem& rhs
    ) {
        return lhs.id() == rhs.id();
    }

    friend auto operator!=( const CoordSystem& lhs,
         const CoordSystem& rhs
    ) {
        return lhs.id() != rhs.id();
    }

private:
    void VCALL setTotal(Transform total) {
        total_ = total;
    }

    void detachThisFromParent();
    static void linkParentChild(const CoordSystem& parent,
        CoordSystem& child
    );

    BasicTransformComponent tc_;
    std::optional<Transform> total_;
    std::optional<const CoordSystem*> parent_;
    // children_ should be mutable to reflect destruction
    // of current CoordSystem on parent_
    // (via detaching parent's child which is equal to this)
    // If children are too many, consider to change data structure.
    mutable std::vector<CoordSystem*> children_;
    CoordSystemID id_;
};

namespace detail {

template <class TransFn>
class CoordBase {
public:
    CoordBase()
        : abs_(dx::XMVectorZero()), rep_(dx::XMVectorZero()), base_() {}

    CoordBase(dx::XMVECTOR absolute)
        : abs_(absolute), rep_(absolute), base_() {}

    CoordBase(const CoordSystem& base)
        : CoordBase(base, dx::XMVectorZero()) {}

    CoordBase(const CoordSystem& base, dx::FXMVECTOR representation)
        : abs_(), rep_(representation), base_(&base) {
            calcAbsFromRep();
    }

    void adapt(const CoordSystem& cs) {
        base_ = &cs;
        calcRepFromAbs();
    }

    void VCALL setRep(dx::FXMVECTOR val) noexcept {
        rep_ = val;
        calcAbsFromRep();
    }

    const dx::XMVECTOR VCALL rep() const noexcept {
        return rep_;
    }

    void VCALL setAbs(dx::FXMVECTOR val) noexcept {
        abs_ = val;
        calcRepFromAbs();
    }

    const dx::XMVECTOR VCALL abs() const noexcept {
        return rep_;
    }

    void update() {
        calcAbsFromRep();
    }

protected:
    void calcAbsFromRep() {
        if (!base_.has_value()) [[unlikely]] {
            abs_ = rep_;
        }

        abs_ = std::invoke(TransFn{}, rep_, base_.value()->total().get());
    }

    void calcRepFromAbs() {
        if (!base_.has_value()) [[unlikely]] {
            rep_ = abs_;
        }

        auto inv = dx::XMMatrixInverse(nullptr, base_.value()->total().get());
        rep_ = std::invoke(TransFn{}, abs_, inv);
    }

    dx::XMVECTOR abs_;
    dx::XMVECTOR rep_;
    std::optional<const CoordSystem*> base_;
};

struct CoordPtTrans {
    dx::XMVECTOR VCALL operator()(
        dx::FXMVECTOR vec, dx::FXMMATRIX trans
    ) noexcept {
        return dx::XMVector3Transform(vec, trans);
    }
};

struct CoordVecTrans {
    dx::XMVECTOR VCALL operator()(
        dx::FXMVECTOR vec, dx::FXMMATRIX trans
    ) noexcept {
        return dx::XMVector3TransformNormal(vec, trans);
    }
};

}

class CoordPt : public detail::CoordBase< detail::CoordPtTrans >{
public:
    CoordPt()
        : detail::CoordBase< detail::CoordPtTrans >() {}

    CoordPt(dx::XMVECTOR absolute)
        : detail::CoordBase< detail::CoordPtTrans >(absolute) {}

    CoordPt(const CoordSystem& base)
        : detail::CoordBase< detail::CoordPtTrans >(base) {}

    CoordPt(const CoordSystem& base, dx::FXMVECTOR representation)
        : detail::CoordBase< detail::CoordPtTrans >(
            base, representation
        ) {}
};

class CoordVec : public detail::CoordBase< detail::CoordVecTrans >{
public:
    CoordVec()
        : detail::CoordBase< detail::CoordVecTrans >() {}

    CoordVec(dx::XMVECTOR absolute)
        : detail::CoordBase< detail::CoordVecTrans >(absolute) {}

    CoordVec(const CoordSystem& base)
        : detail::CoordBase< detail::CoordVecTrans >(base) {}

    CoordVec(const CoordSystem& base, dx::FXMVECTOR representation)
        : detail::CoordBase< detail::CoordVecTrans >(
            base, representation
        ) {}
};

inline CoordPt coordConv(const CoordPt& coord, const CoordSystem& base) {
    auto ret = coord;
    ret.adapt(base);
    return ret;
}

inline CoordPt coordConv(CoordPt&& coord, const CoordSystem& base) {
    coord.adapt(base);
    return std::move(coord);
}

inline CoordVec coordConv(const CoordVec& coord, const CoordSystem& base) {
    auto ret = coord;
    ret.adapt(base);
    return ret;
}

inline CoordVec coordConv(CoordVec&& coord, const CoordSystem& base) {
    coord.adapt(base);
    return std::move(coord);
}

#endif  // __CoordSystem