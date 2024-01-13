#ifndef __RendererSystem
#define __RendererSystem

#include "Renderer.hpp"
#include "Scene.hpp"

#include "GFX/Core/GraphicsStorage.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <vector>
#include <memory>
#include <tuple>
#include <ranges>
#include <algorithm>

class RendererSystem {
public:
    using Slot = std::size_t;

    RendererSystem(GFXFactory factory, GFXPipeline pipeline)
        : resourceStorage_(), pairs_(),
        factory_( std::move(factory) ),
        pipeline_( std::move(pipeline) ) {}

    template <class RendererT>
    Slot addRenderer() {
        pairs_.emplace_back( std::make_unique<RendererT>(pipeline_), Scene{} );
        return pairs_.size() - 1u;
    }

    void sync(Slot slot) {
        auto& [pRenderer, _] = pairs_.at(slot);
        pRenderer->sync(resourceStorage_, factory_);
    }

#ifdef ACTIVATE_RENDERER_LOG
    void enableLog(Slot slot) noexcept {
        auto& [pRenderer, _] = pairs_.at(slot);
        pRenderer->enableLog();
    }

    void disableLog(Slot slot) noexcept {
        auto& [pRenderer, _] = pairs_.at(slot);
        pRenderer->disableLog();
    }

    bool logEnabled(Slot slot) const noexcept {
        auto& [pRenderer, _] = pairs_.at(slot);
        return pRenderer->logEnabled();
    }
#endif // ACTIVATE_RENDERER_LOG

    void render() {
        for (auto slot = Slot(0); slot < pairs_.size(); ++slot) {
            render(slot);
        }
    }

    void render(Slot slot) {
        auto& [pRenderer, scene] = pairs_.at(slot);
        scene.sortFor(*pRenderer);
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

    GFXStorage& storage() {
        return resourceStorage_;
    }

    template <class SceneAdapter>
    SceneAdapter adapt(Slot slot) {
        return adapt<SceneAdapter>( scene(slot) );
    }

    template <class SceneAdapter>
    const SceneAdapter adapt(Slot slot) const {
        return adapt<SceneAdapter>( scene(slot) );
    }

    template <class SceneAdapter>
    static SceneAdapter adapt(Scene& scene) {
        return SceneAdapter(scene);
    }

    template <class SceneAdapter>
    static const SceneAdapter adapt(const Scene& scene) {
        return SceneAdapter(scene);
    }

private:
    GFXStorage resourceStorage_;
    std::vector< std::pair<
        std::unique_ptr<Renderer>, Scene
    > > pairs_; // Renderer - Scene pairs (1:1 relationship)
    GFXFactory factory_;
    GFXPipeline pipeline_;
};

#endif  // __RendererSystem