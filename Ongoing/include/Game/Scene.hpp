#ifndef __Scene
#define __Scene

#include "DrawComponent.hpp"
#include "Camera.hpp"

#include "GFX/Core/GraphicsStorage.hpp"

#include <ranges>
#include <algorithm>
#include <vector>
#include <memory>

#include "AdditionalRanges.hpp"

#include <cassert>

class Scene {
public:
    void addDrawComponent(std::shared_ptr<IDrawComponent> pDC) {
        assert( std::ranges::find(pDCs_, pDC) == pDCs_.end() );

        pDCs_.push_back( std::move(pDC) );
    }

    decltype(auto) drawComponents() noexcept {
        return pDCs_ | dereference();
    }

    decltype(auto) drawComponents() const noexcept {
        return pDCs_ | dereference();
    }

    template <class TRenderer>
    void sortFor(const TRenderer& renderer) {
        std::ranges::for_each( drawComponents(), [&renderer](auto& dc) {
            dc.sync(renderer);
        } );
        std::ranges::sort( pDCs_, std::less<>{},
            &IDrawComponent::renderObjectDesc
        );
    }

    const CameraVision& vision() const noexcept {
        return *pVision_;
    }

    void setVision(std::shared_ptr<CameraVision> pVision) {
        pVision_ = pVision;
    }

private:
    std::vector< std::shared_ptr<IDrawComponent> > pDCs_;
    std::shared_ptr<CameraVision> pVision_;
};

#endif  // __Scene