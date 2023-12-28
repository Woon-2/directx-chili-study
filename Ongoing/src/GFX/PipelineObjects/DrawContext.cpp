#include "GFX/PipelineObjects/DrawContext.hpp"

#include "GFX/Core/Pipeline.hpp"
#include "GFX/Core/GraphicsException.hpp"

void DrawContextBasic::basicDrawCall(GFXPipeline& pipeline) const {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->Draw(numVertex_, startVertexLocation_)
    );

#ifdef ACTIVATE_DRAWCONTEXT_LOG
    logComponent_.logDraw();
#endif
}

void DrawContextBasic::drawCall(GFXPipeline& pipeline) const {
    basicDrawCall(pipeline);
}

void DrawContextIndexed::indexedDrawCall(GFXPipeline& pipeline) const {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->DrawIndexed(numIndex_,
            startIndexLocation_, baseVertexLocation_
        )
    );

#ifdef ACTIVATE_DRAWCONTEXT_LOG
    logComponent_.logDraw();
#endif
}

void DrawContextIndexed::drawCall(GFXPipeline& pipeline) const {
    indexedDrawCall(pipeline);
}
