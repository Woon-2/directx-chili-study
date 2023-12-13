#ifndef __Scene
#define __Scene

#include "DrawComponent.hpp"

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

private:
    std::vector< std::shared_ptr<IDrawComponent> > pDCs_;
};

#endif  // __Scene