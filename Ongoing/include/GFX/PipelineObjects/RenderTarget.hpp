#ifndef __RenderTarget
#define __RenderTarget

#include "Bindable.hpp"
#include "GFX/Core/Pipeline.hpp"
#include "GFX/Core/GFXFactory.hpp"

#include <d3d11.h>
#include "GFX/Core/GraphicsNamespaces.hpp"

class RenderTarget : public IBindable {
public:
    RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer);

    RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer,
        D3D11_RENDER_TARGET_VIEW_DESC RTVDesc
    );

    RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer,
        ID3D11Resource* pDSBuffer, D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc
    );

    RenderTarget(GFXFactory factory, ID3D11Resource* pRTBuffer,
        D3D11_RENDER_TARGET_VIEW_DESC RTVDesc,
        ID3D11Resource* pDSBuffer,
        D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc
    );

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

    void clear(dx::FXMVECTOR color, float depth);

private:
    static constexpr auto defClearDepth = 1.f;
    // since XMFLOAT4 is loaded to XMVECTOR by its pointer,
    // defClearColor must be addressable, so it cannot be constexpr.
    static const dx::XMFLOAT4 defClearColor;

    void bind(GFXPipeline& pipeline) override final;

    void linkPipeline(GFXPipeline pipeline) noexcept {
        pipeline_ = pipeline;
    }

    GFXPipeline pipeline_;
    wrl::ComPtr<ID3D11RenderTargetView> pRTV_;
    wrl::ComPtr<ID3D11DepthStencilView> pDSV_;
};

#endif  // __RenderTarget