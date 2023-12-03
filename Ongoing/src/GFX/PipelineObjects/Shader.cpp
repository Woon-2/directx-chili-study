#include "GFX/PipelineObjects/Shader.hpp"

void VertexShader::bind(GFXPipeline& pipeline) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->VSSetShader(
            pVertexShader_.Get(), nullptr, 0
        )
    );
    GFX_THROW_FAILED_VOID(
        pipeline.context()->IASetInputLayout(
            pInputLayout_.Get()
        )
    );
}

PixelShader::PixelShader( GFXFactory factory,
    const std::filesystem::path& path
) {
    GFX_THROW_FAILED(
        D3DReadFileToBlob(path.c_str(), &byteCode_)
    );
    GFX_THROW_FAILED(
        factory.device()->CreatePixelShader( byteCode(),
            byteCodeLength(), nullptr, &pPixelShader_
        )
    );
}

void PixelShader::bind(GFXPipeline& pipeline) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->PSSetShader(
            pPixelShader_.Get(), nullptr, 0u
        )
    );
}