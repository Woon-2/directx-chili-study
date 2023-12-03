#ifndef __Scene
#define __Scene

#include "DrawComponent.hpp"

#include <ranges>
#include <algorithm>
#include <vector>
#include <memory>

#include "GFX/Core/GraphicsStorage.hpp"

#include "AdditionalRanges.hpp"

class Scene {
public:
    template <class T>
    void addDrawComponent(T* pDC) {
        pDCs_.push_back( std::shared_ptr<IDrawComponent>(pDC) );
    }

    void addDrawComponent(std::shared_ptr<IDrawComponent> pDC) {
        pDCs_.push_back( std::move(pDC) );
    }

    template <class RendererT>
    void sortFor(const RendererT& renderer) {
        std::ranges::sort(pDCs_, std::less<>{},
            &IDrawComponent::renderDesc
        );
    }

    GFXStorage& storage() noexcept {
        return storage_;
    }

    decltype(auto) drawComponents() noexcept {
        return pDCs_ | dereference();
    }

    decltype(auto) drawComponents() const noexcept {
        return pDCs_ | dereference();
    }

private:
    GFXStorage storage_;
    std::vector< std::shared_ptr<IDrawComponent> > pDCs_;
};

#endif  // __Scene