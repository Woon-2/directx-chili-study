#include "Game/CoordSystem.hpp"

CoordSystemID CoordSystemID::gen() {
    static auto IDDistribution = std::size_t(0u); 
    // maybe occur overflow.
    // subsitute this with robust algorithm later.
    return CoordSystemID(IDDistribution++);
}

void CoordSystem::setParent(std::shared_ptr<const CoordSystem> parent) {
    if (!lostParent()) {
        loseParent();
    }

    auto parentInversion = dx::XMMatrixInverse(
        nullptr, parent->tc_->total().get()
    );

    tc_->setGlobal( parentInversion * tc_->globalRef() );

    parent_ = parent;
}

void CoordSystem::loseParent() {
    if (lostParent()) {
        return;
    }

    tc_->setGlobal( parent_.value()->tc_->total() * tc_->globalRef() );
    parent_.reset();
}

void CoordSystem::traverse() {
    // update it's own total transform (world transform)
    if (lostParent()) {
        tc_->setTotal( tc_->local() * tc_->global() );
    }
    else {
        tc_->setTotal( tc_->local()
            * parent_.value()->tc_->total()
            * tc_->global()
        );
    }

    // traverse children
    std::ranges::for_each( children_, [](auto& wpChild) {
        if (auto spChild = wpChild.lock()) {
            spChild->traverse();
        }
    } );

    removeDetachedChildren();
}

void CoordSystem::removeDetachedChildren() {
    auto eraseRange = std::ranges::remove_if( children_,
        [](auto& wpChild) {
            return wpChild.expired();
        }
    );

    children_.erase(eraseRange.begin(), eraseRange.end());
}