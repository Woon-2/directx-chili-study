#include "GFX/PipelineObjects/DrawContext.hpp"

#include "GFX/Core/Pipeline.hpp"
#include "GFX/Core/GraphicsException.hpp"

void DrawContextIndexed::drawCall(GFXPipeline& pipeline) const {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->DrawIndexed(numIndex_,
            startIndexLocation_, baseVertexLocation_
        )
    );
}
