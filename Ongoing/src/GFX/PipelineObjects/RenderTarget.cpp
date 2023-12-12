#include "GFX/PipelineObjects/RenderTarget.hpp"

#include "GFX/Core/GraphicsException.hpp"

void RenderTargetBinder::doBind(GFXPipeline& pipeline,
    ID3D11RenderTargetView** pRTVs, ID3D11DepthStencilView* pDSV
) {
    GFX_THROW_FAILED_VOID(
        pipeline.context()->OMSetRenderTargets(
            1u, pRTVs, pDSV
        )
    );
}

RenderTarget::RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer)
    : pipeline_(), pRTV_(), pDSV_() {
    GFX_THROW_FAILED(
        factory.device()->CreateRenderTargetView(
            pRTBuffer, nullptr, &pRTV_
        )
    );
}

RenderTarget::RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer,
    D3D11_RENDER_TARGET_VIEW_DESC RTVDesc
) : pipeline_(), pRTV_(), pDSV_() {
    GFX_THROW_FAILED(
        factory.device()->CreateRenderTargetView(
            pRTBuffer, &RTVDesc, &pRTV_
        )
    );
}

RenderTarget::RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer,
    ID3D11Resource* pDSBuffer, D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc
) : pipeline_(), pRTV_(), pDSV_() {
    GFX_THROW_FAILED(
        factory.device()->CreateRenderTargetView(
            pRTBuffer, nullptr, &pRTV_
        )
    );

    GFX_THROW_FAILED(
        factory.device()->CreateDepthStencilView(
            pDSBuffer, &DSVDesc, &pDSV_
        )
    );
}

RenderTarget::RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer,
    D3D11_RENDER_TARGET_VIEW_DESC RTVDesc,
    ID3D11Resource* pDSBuffer,
    D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc
) : pipeline_(), pRTV_(), pDSV_() {
    GFX_THROW_FAILED(
        factory.device()->CreateRenderTargetView(
            pRTBuffer, &RTVDesc, &pRTV_
        )
    );

    GFX_THROW_FAILED(
        factory.device()->CreateDepthStencilView(
            pDSBuffer, &DSVDesc, &pDSV_
        )
    );
}

void RenderTarget::clear(dx::FXMVECTOR color, float depth) {
    dx::XMFLOAT4 tmp;
    dx::XMStoreFloat4(&tmp, color);

    // Due to the signature of CllarRenderTargetView,
    // which receives colors only through float built-in array,
    // convert the color vector to float built-in array.
    float colorArr[4] = { tmp.x, tmp.y, tmp.z, tmp.w };
    
    GFX_THROW_FAILED_VOID(
        pipeline_.context()->ClearRenderTargetView(
            pRTV_.Get(), colorArr
        )
    );

    if (pDSV_) {
        GFX_THROW_FAILED_VOID(
            pipeline_.context()->ClearDepthStencilView(
                pDSV_.Get(), D3D11_CLEAR_DEPTH, depth, 0u
            )
        );
    }
}

void RenderTarget::bind(GFXPipeline& pipeline) {
    // should store pipeline for later use of it in clear function.
    linkPipeline(pipeline);

    binder_.bind( pipeline, pRTV_.GetAddressOf(), pDSV_.Get() );
}

const dx::XMFLOAT4 RenderTarget::defClearColor
    = dx::XMFLOAT4(0.f, 0.f, 0.f, 1.f);