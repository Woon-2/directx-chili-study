#ifndef __Renderer
#define __Renderer

#include "Scene.hpp"

#include "GFX/Core/Pipeline.hpp"

class Renderer {
public:
    void linkPipeline(GFXPipeline& pipeline) {
        pipeline_ = pipeline;
    }

    void linkPipeline(GFXPipeline&& pipeline) {
        pipeline_ = std::move(pipeline);
    }

    void render(Scene& scene);

private:
    GFXPipeline pipeline_;
};

#endif  // __Renderer