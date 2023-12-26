#include "GFX/PipelineObjects/Texture.hpp"

void TextureBinder::doBind( GFXPipeline& pipeline, std::size_t slot,
    ID3D11ShaderResourceView** srvArr
) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->PSSetShaderResources(
            slot, 1u, srvArr
        )
    );
}

Texture::Texture( GFXFactory factory, std::size_t slot,
    const Surface& surface
#ifdef ACTIVATE_BINDABLE_LOG
    , bool enableLogOnCreation
#endif
) :
#ifdef ACTIVATE_BINDABLE_LOG
    logComponent_( this, GFXCMDSourceCategory("Texture") ),
#endif
    slot_(slot), pSRV_() {
    auto textureDesc = D3D11_TEXTURE2D_DESC{
        .Width = static_cast<UINT>( surface.GetWidth() ),
        .Height = static_cast<UINT>( surface.GetHeight() ),
        .MipLevels = 1u,
        .ArraySize = 1u,
        .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
        .SampleDesc = DXGI_SAMPLE_DESC{
            .Count = 1u,
            .Quality = 0u
        },
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_SHADER_RESOURCE,
        .CPUAccessFlags = 0u,
        .MiscFlags = 0u
    };

    auto sd = D3D11_SUBRESOURCE_DATA{
        .pSysMem = surface.GetBufferPtr(),
        .SysMemPitch = static_cast<UINT>(
            surface.GetWidth() * sizeof( Surface::Color )
        )
    };

    auto pTexture = wrl::ComPtr<ID3D11Texture2D>();

    GFX_THROW_FAILED(
        factory.device()->CreateTexture2D(
            &textureDesc, &sd, &pTexture
        )
    );

    auto srvDesc = D3D11_SHADER_RESOURCE_VIEW_DESC{
        .Format = textureDesc.Format,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        .Texture2D = D3D11_TEX2D_SRV{
            .MostDetailedMip = 0u,
            .MipLevels = 1u
        }
    };

    GFX_THROW_FAILED(
        factory.device()->CreateShaderResourceView(
            pTexture.Get(), &srvDesc, &pSRV_
        )
    );

#ifdef ACTIVATE_BINDABLE_LOG
    if (enableLogOnCreation) {
        logComponent_.enableLog();
    }
    logComponent_.logCreate();
#endif
}

void Texture::bind(GFXPipeline& pipeline) {
    [[maybe_unused]] auto bBindOccured = binder_.bind(
        pipeline, slot_, pSRV_.GetAddressOf()
    );

#ifdef ACTIVATE_BINDABLE_LOG
    if (bBindOccured) {
        logComponent_.logBind();
    }
#endif 
}