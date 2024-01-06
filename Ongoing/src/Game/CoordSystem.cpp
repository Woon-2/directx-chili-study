#include "Game/CoordSystem.hpp"

CoordSystemID CoordSystemID::gen() {
    static auto IDDistribution = std::size_t(0u); 
    // maybe occur overflow.
    // subsitute this with robust algorithm later.
    return CoordSystemID(IDDistribution++);
}

CoordSystem::~CoordSystem() {
    if (!lostParent()) {
        detachThisFromParent();
        std::ranges::for_each( children_, [this](auto* pChild) {
            pChild->setParent(*parent_.value());
        } );
    }
    else {
        std::ranges::for_each( children_, [](auto* pChild) {
            pChild->loseParent();
        } );
    }
}

void CoordSystem::loseParent() {
    if (lostParent()) {
        return;
    }

    detachThisFromParent();

    setGlobal( parent_.value()->total() * global() );
    parent_.reset();
    setDirty();
}

void CoordSystem::traverse() {
    if (dirty()) {
        // update it's own total transform (world transform)
        if (lostParent()) {
            setTotal( tc_.local() * tc_.global() );
        }
        else {
            setTotal( tc_.local()
                * parent_.value()->total()
                * tc_.global()
            );
        }
    }

    // traverse children
    std::ranges::for_each( children_, [this](auto* pChild) {
        if (dirty()) {
            pChild->setDirty();
        }
        pChild->traverse();
    } );
}

void CoordSystem::destroyCascade() noexcept {
    if (!lostParent()) {
        detachThisFromParent();
    }

    std::ranges::for_each( children_, [](auto* pChild) {
        pChild->destroyCascade();
    } );

    parent_.reset();
    children_.clear();
}

void CoordSystem::detachThisFromParent() {
    auto& parentChildren = parent_.value()->children_;
    // since std::ranges::remove returns subrange,
    // take the subrange.begin() as pos and ignore the subrange.end().
    auto [pos, _] = std::ranges::remove( parent_.value()->children_, this );
    parentChildren.erase(pos);
}

void CoordSystem::linkParentChild(
    const CoordSystem& parent, CoordSystem& child
) {
    if (!child.lostParent()) {
        child.loseParent();
    }

    auto parentInversion = dx::XMMatrixInverse(
        nullptr, parent.total().get()
    );

    child.setGlobal( parentInversion * child.global() );
    child.setDirty();

    child.parent_ = &parent;
    parent.children_.push_back(&child);
}