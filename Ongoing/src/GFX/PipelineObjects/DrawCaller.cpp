#include "GFX/PipelineObjects/DrawCaller.hpp"

#include "GFX/Core/Pipeline.hpp"
#include "GFX/Core/GraphicsException.hpp"

void DrawCaller::basicDrawCall(GFXPipeline& pipeline) const {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->Draw(numVertex_, startVertexLocation_)
    );

#ifdef ACTIVATE_DRAWCALLER_LOG
    logComponent_.logDraw();
#endif
}

void DrawCaller::drawCall(GFXPipeline& pipeline) const {
    basicDrawCall(pipeline);
}

void DrawCallerIndexed::indexedDrawCall(GFXPipeline& pipeline) const {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->DrawIndexed(numIndex_,
            startIndexLocation_, baseVertexLocation_
        )
    );

#ifdef ACTIVATE_DRAWCALLER_LOG
    logComponent_.logDraw();
#endif
}

void DrawCallerIndexed::drawCall(GFXPipeline& pipeline) const {
    indexedDrawCall(pipeline);
}
