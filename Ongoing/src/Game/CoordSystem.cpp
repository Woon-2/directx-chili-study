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

void CoordSystem::setParent(const CoordSystem& parent) {
    if (!lostParent()) {
        loseParent();
    }

    auto parentInversion = dx::XMMatrixInverse(
        nullptr, parent.tc_.total().get()
    );

    tc_.setGlobal( parentInversion * tc_.globalRef() );

    parent_ = &parent;
    dirty_ = true;
}

void CoordSystem::loseParent() {
    if (lostParent()) {
        return;
    }

    detachThisFromParent();

    tc_.setGlobal( parent_.value()->tc_.total() * tc_.globalRef() );
    parent_.reset();
    dirty_ = true;
}

void CoordSystem::traverse() {
    if (dirty_) {
        // update it's own total transform (world transform)
        if (lostParent()) {
            tc_.setTotal( tc_.local() * tc_.global() );
        }
        else {
            tc_.setTotal( tc_.local()
                * parent_.value()->tc_.total()
                * tc_.global()
            );
        }
    }

    // traverse children
    std::ranges::for_each( children_, [this](auto* pChild) {
        if (dirty_) {
            pChild->setDirty();
        }
        pChild->traverse();
    } );

    dirty_ = false;
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