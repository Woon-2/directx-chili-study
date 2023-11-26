#ifndef __Scene
#define __Scene

#include "DrawComponent.hpp"
#include "GraphicsStorage.hpp"

#include <ranges>
#include <algorithm>
#include <vector>
#include <memory>

#include "AdditionalRanges.hpp"

class Scene {
public:
    template <class T>
    void addDrawComponent(T* pDC) {
        pDCs_.push_back(std::unique_ptr<IDrawComponent>(pDC));
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
    std::vector< std::unique_ptr<IDrawComponent> > pDCs_;
};

class ILoader {
public:
    virtual ~ILoader() {}

    virtual void loadAt(Scene& scene) = 0;
};

// Loader is responsible for adding entities at a scene.
// Like DrawComponent<T> class template,
// it stands for having constraints of naming.
// So prevent instantiation with general type T.
// Loader has to be specialized.
template <class T>
class Loader;

#endif  // __Scene