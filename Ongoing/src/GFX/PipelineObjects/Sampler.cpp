#include "GFX/PipelineObjects/Sampler.hpp"

void SamplerBinder::doBind( GFXPipeline& pipeline,
    UINT slot, ID3D11SamplerState* pSampler
) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->PSSetSamplers(slot, 1u, &pSampler)
    );
}

Sampler::Sampler( GFXFactory factory
#ifdef ACTIVATE_BINDABLE_LOG
    , bool enableLogOnCreation
#endif
) :
#ifdef ACTIVATE_BINDABLE_LOG
    logComponent_( this, GFXCMDSourceCategory("Texture") ),
#endif
    slot_(), pSampler_() {
    auto samplerDesc = D3D11_SAMPLER_DESC{
        .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressW = D3D11_TEXTURE_ADDRESS_WRAP
    };

    GFX_THROW_FAILED(
        factory.device()->CreateSamplerState(
            &samplerDesc, &pSampler_
        )
    );
}

void Sampler::bind(GFXPipeline& pipeline) {
    [[maybe_unused]] auto bBindOccured = binder_.bind( slot_,
        pipeline, slot_, pSampler_.Get()
    );

    #ifdef ACTIVATE_BINDABLE_LOG
        if (bBindOccured) {
            logComponent_.logBind();
        }
    #endif 
}