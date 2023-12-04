#include "Game/Renderer.hpp"

#include "Game/RenderDesc.hpp"
#include "GFX/PipelineObjects/Bindable.hpp"

#include <ranges>
#include <algorithm>

void Renderer::render(Scene& scene) {
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