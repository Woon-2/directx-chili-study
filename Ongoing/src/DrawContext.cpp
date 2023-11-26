#include "DrawContext.hpp"
#include "Pipeline.hpp"
#include "GraphicsException.hpp"

void DrawContextIndexed::drawCall(GFXPipeline& pipeline) const {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->DrawIndexed(numIndex_,
            startIndexLocation_, baseVertexLocation_
        )
    );
}
