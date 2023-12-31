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
        : parent_(), tc_(std::make_shared<BasicTransformComponent>()),
        children_(), id_() {}

    void addChild(std::shared_ptr<CoordSystem> child) {
        children_.push_back( std::move(child) );
    }

    void setParent(std::shared_ptr<const CoordSystem> parent);
    void loseParent();
    bool lostParent() const noexcept {
        return !parent_.has_value();
    }
    void traverse();

    const CoordSystemID id() const noexcept {
        return id_;
    }

    void VCALL adjustLocal(const Transform trans) noexcept {
        tc_->adjustLocal(trans);
    }

    void VCALL setLocal(const Transform trans) noexcept {
        tc_->setLocal(trans);
    }

    const Transform local() const noexcept {
        return tc_->local();
    }

    Transform& localRef() noexcept {
        return tc_->localRef();
    }

    const Transform& localRef() const noexcept {
        return tc_->localRef();
    }

    void VCALL adjustGlobal(const Transform trans) noexcept {
        tc_->adjustGlobal(trans);
    }

    void VCALL setGlobal(const Transform trans) noexcept {
        tc_->setGlobal(trans);
    }

    const Transform global() const noexcept {
        return tc_->global();
    }

    Transform& globalRef() noexcept {
        return tc_->globalRef();
    }

    const Transform& globalRef() const noexcept {
        return tc_->globalRef();
    }

    const Transform& total() const noexcept {
        return tc_->total();
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
    void removeDetachedChildren();

    std::optional<
        std::shared_ptr<const CoordSystem>
    > parent_;
    std::shared_ptr<BasicTransformComponent> tc_;
    std::vector< std::weak_ptr<CoordSystem> > children_;
    CoordSystemID id_;
};

#endif  // __CoordSystem