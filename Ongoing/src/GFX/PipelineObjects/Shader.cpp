#include "GFX/PipelineObjects/Shader.hpp"

void VertexShaderBinder::doBind(GFXPipeline& pipeline,
    ID3D11VertexShader* pShader, ID3D11InputLayout* pIA
) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->VSSetShader(
            pShader, nullptr, 0
        )
    );
    GFX_THROW_FAILED_VOID(
        pipeline.context()->IASetInputLayout(pIA)
    );
}

void VertexShader::bind(GFXPipeline& pipeline) {
    binder_.bind( pipeline, pVertexShader_.Get(), pInputLayout_.Get() );
}

void PixelShaderBinder::doBind(
    GFXPipeline& pipeline, ID3D11PixelShader* pShader
) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->PSSetShader(
            pShader, nullptr, 0u
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
    binder_.bind( pipeline, pPixelShader_.Get() );
}