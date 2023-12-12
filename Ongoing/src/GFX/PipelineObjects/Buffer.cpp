#include "GFX/PipelineObjects/Buffer.hpp"

void VertexBufferBinder::doBind( GFXPipeline& pipeline,
    ID3D11Buffer** pBuffers, const UINT* strides, const UINT* offsets
) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->IASetVertexBuffers(
            0u, 1u, pBuffers, strides, offsets
        )
    );
}

void IndexBufferBinder::doBind( GFXPipeline& pipeline,
    ID3D11Buffer* pBuffer, DXGI_FORMAT indexFormat
) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->IASetIndexBuffer(
            pBuffer, indexFormat, 0u
        )
    );
}

void VSCBufferBinder::doBind(GFXPipeline& pipeline,
    UINT slot, ID3D11Buffer** pBuffers
) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->VSSetConstantBuffers(
            slot, 1u, pBuffers
        )
    );
}

void PSCBufferBinder::doBind( GFXPipeline& pipeline,
    UINT slot, ID3D11Buffer** pBuffers
) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->PSSetConstantBuffers(
            slot, 1u, pBuffers
        )
    );
}