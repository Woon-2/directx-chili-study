#ifndef __RenderTarget
#define __RenderTarget

#include "Bindable.hpp"
#include "Pipeline.hpp"
#include "GFXFactory.hpp"

#include <d3d11.h>
#include "GraphicsNamespaces.hpp"
#include "GraphicsException.hpp"

class RenderTarget : public IBindable {
public:
    RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer)
        : pipeline_(), pRTV_(), pDSV_() {
        GFX_THROW_FAILED(
            factory.device()->CreateRenderTargetView(
                pRTBuffer, nullptr, &pRTV_
            )
        );
    }

    RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer,
        D3D11_RENDER_TARGET_VIEW_DESC RTVDesc
    ) : pipeline_(), pRTV_(), pDSV_() {
        GFX_THROW_FAILED(
            factory.device()->CreateRenderTargetView(
                pRTBuffer, &RTVDesc, &pRTV_
            )
        );
    }

    RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer,
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

    RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer,
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

    void clear() {
        clear(defClearDepth);
    }

    void clear(float r, float g, float b, float a) {
        clear(r, g, b, a, defClearDepth);
    }

    void clear(dx::FXMVECTOR color) {
        clear(color, defClearDepth);
    }

    void clear(float depth) {
        auto color = dx::XMLoadFloat4(&defClearColor);
        clear(color, depth);
    }

    void clear(float r, float g, float b, float a, float depth) {
        auto tmp = dx::XMFLOAT4(r, g, b, a);
        auto color = dx::XMLoadFloat4(&tmp);
        clear(color, depth);
    }

    void clear(dx::FXMVECTOR color, float depth) {
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

private:
    static constexpr auto defClearDepth = 1.f;
    // since XMFLOAT4 is loaded to XMVECTOR by its pointer,
    // defClearColor must be addressable, so it cannot be constexpr.
    static const dx::XMFLOAT4 defClearColor;

    void bind(GFXPipeline& pipeline) override {
        // should store pipeline for later use of it in clear function.
        linkPipeline(pipeline);

        GFX_THROW_FAILED_VOID(
            pipeline_.context()->OMSetRenderTargets(1u,
                pRTV_.GetAddressOf(), pDSV_.Get()
            )
        );
    }

    void linkPipeline(GFXPipeline pipeline) noexcept {
        pipeline_ = pipeline;
    }

    GFXPipeline pipeline_;
    wrl::ComPtr<ID3D11RenderTargetView> pRTV_;
    wrl::ComPtr<ID3D11DepthStencilView> pDSV_;
};

const dx::XMFLOAT4 RenderTarget::defClearColor
    = dx::XMFLOAT4(0.f, 0.f, 0.f, 1.f);

#endif  // __RenderTarget