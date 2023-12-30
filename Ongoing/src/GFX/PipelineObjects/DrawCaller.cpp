#include "GFX/PipelineObjects/DrawCaller.hpp"

#include "GFX/Core/Pipeline.hpp"
#include "GFX/Core/GraphicsException.hpp"

void DrawCallerBasic::basicDrawCall(GFXPipeline& pipeline) const {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->Draw(numVertex_, startVertexLocation_)
    );

#ifdef ACTIVATE_DRAWCALLER_LOG
    logComponent_.logDraw();
#endif
}

void DrawCallerBasic::drawCall(GFXPipeline& pipeline) const {
    beforeDrawCall(pipeline);
    basicDrawCall(pipeline);
    afterDrawCall(pipeline);
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
    beforeDrawCall(pipeline);
    indexedDrawCall(pipeline);
    afterDrawCall(pipeline);
}
