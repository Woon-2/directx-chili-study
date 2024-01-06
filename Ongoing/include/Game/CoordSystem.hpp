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

#endif  // __CoordSystem