#ifndef __Renderer
#define __Renderer

#include "Bindable.hpp"
#include "Pipeline.hpp"
#include "Scene.hpp"
#include "RenderDesc.hpp"

#include <ranges>
#include <algorithm>

class Renderer {
public:
    void linkPipeline(GFXPipeline& pipeline) {
        pipeline_ = pipeline;
    }

    void linkPipeline(GFXPipeline&& pipeline) {
        pipeline_ = std::move(pipeline);
    }

    void render(Scene& scene) {
        std::ranges::for_each(scene.drawComponents(),
            [&scene, this, prev = decltype(RenderDesc::IDs)()](const auto& dc) mutable {
            auto cur = std::move(dc.renderDesc().IDs);

            std::ranges::for_each(cur,
                [&](auto&& id) {
                    if (!prev.contains(id)) {
                        pipeline_.bind(scene.storage().get(
                            std::forward<decltype(id)>(id)
                        ).value() );
                    }
                }
            );

            pipeline_.drawCall(dc.drawContext());
            prev = std::move(cur);
        });
    }

private:
    GFXPipeline pipeline_;
};

#endif  // __Renderer