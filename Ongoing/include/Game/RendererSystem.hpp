#ifndef __RendererSystem
#define __RendererSystem

#include "Renderer.hpp"
#include "Scene.hpp"

#include <vector>
#include <memory>
#include <tuple>
#include <ranges>
#include <algorithm>

class RendererSystem {
public:
    using Slot = std::size_t;

    Slot addRenderer(std::unique_ptr<Renderer>&& renderer) {
        pairs_.emplace_back( std::move(renderer), Scene{} );
        return pairs_.size() - 1u;
    }

    void render() {
        for (auto slot = Slot(0); slot < pairs_.size(); ++slot) {
            render(slot);
        }
    }

    void render(Slot slot) {
        auto& [pRenderer, scene] = pairs_.at(slot);
        pRenderer->render(scene);
    }

    Renderer& renderer(Slot slot) {
        return *(pairs_.at(slot).first);
    }

    const Renderer& renderer(Slot slot) const {
        return *(pairs_.at(slot).first);
    }

    Scene& scene(Slot slot) {
        return pairs_.at(slot).second;
    }

    const Scene& scene(Slot slot) const {
        return pairs_.at(slot).second;
    }

private:
    std::vector< std::pair<
        std::unique_ptr<Renderer>, Scene
    > > pairs_; // Renderer - Scene pairs (1:1 relationship)
};

#endif  // __RendererSystem