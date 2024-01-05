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
        : tc_(), parent_(),
        children_(), id_(), dirty_(true) {}

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
        dirty_ = true;
    }

    void VCALL setLocal(const Transform trans) noexcept {
        tc_.setLocal(trans);
        dirty_ = true;
    }

    const Transform local() const noexcept {
        return tc_.local();
    }

    Transform& localRef() noexcept {
        dirty_ = true;
        return tc_.localRef();
    }

    const Transform& localRef() const noexcept {
        return tc_.localRef();
    }

    const Transform& localConstRef() const noexcept {
        return tc_.localRef();
    }

    void VCALL adjustGlobal(const Transform trans) noexcept {
        tc_.adjustGlobal(trans);
        dirty_ = true;
    }

    void VCALL setGlobal(const Transform trans) noexcept {
        tc_.setGlobal(trans);
        dirty_ = true;
    }

    const Transform global() const noexcept {
        return tc_.global();
    }

    Transform& globalRef() noexcept {
        dirty_ = true;
        return tc_.globalRef();
    }

    const Transform& globalRef() const noexcept {
        return tc_.globalRef();
    }

    const Transform& globalConstRef() const noexcept {
        return tc_.globalRef();
    }

    void setDirty() noexcept {
        dirty_ = true;
    }

    void destroyCascade() noexcept;

    // total() doesn't intended to be modifiable by client code
    // because it can lead confusion to users whether total is set or not.
    // travarse() is the only one function responsible for setting total transform.
    const Transform& total() const noexcept {
        return tc_.total();
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
    void detachThisFromParent();
    static void linkParentChild(const CoordSystem& parent,
        CoordSystem& child
    );

    BasicTransformComponent tc_;
    std::optional<const CoordSystem*> parent_;
    // children_ should be mutable to reflect destruction
    // of current CoordSystem on parent_
    // (via detaching parent's child which is equal to this)
    // If children are too many, consider to change data structure.
    mutable std::vector<CoordSystem*> children_;
    CoordSystemID id_;
    bool dirty_;
};

#endif  // __CoordSystem