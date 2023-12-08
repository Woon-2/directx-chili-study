#include "GFX/Core/Graphics.hpp"

#include "GFX/PipelineObjects/RenderTarget.hpp"

#include <d3d11.h>
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

#include <cassert>

Graphics::Graphics(MyWindow& wnd)
    : wnd_(wnd), storage_(), factory_(), pipeline_(),
    swapchain_(nullptr), IDAppRenderTarget_() {
    try {
        initGFXComponents();
        constructAppRenderTarget();
    }
    catch (const GraphicsException& e) {
        MessageBoxA(nullptr, e.what(), "Graphics Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const Win32::WindowException& e) {
        MessageBoxA(nullptr, e.what(), "Window Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Standard Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...) {
        MessageBoxA(nullptr, "no details available",
            "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
}

void Graphics::present() {
    swapchain_.present();
    if (swapchain_.deviceRemoved()) {
        throw GFX_DEVICE_REMOVED_EXCEPT( 
            factory_.device()->GetDeviceRemovedReason() 
        );
    }

    // https://devblogs.microsoft.com/directx/dxgi-flip-model/
    // When using flip swap-effect model,
    // After Present calls,
    // the back buffer needs to explicitly be re-bound
    // to the D3D11 immediate context before it can be used again.
    assert( storage_.get(IDAppRenderTarget_).has_value() );
    pipeline_.bind( storage_.get(IDAppRenderTarget_).value() );
}

void Graphics::clear(float r, float g, float b) {
    if ( auto rt = storage_.get(IDAppRenderTarget_) ) {
        static_cast<RenderTarget*>( rt.value() )->clear(r, g, b, 1.f);
    }
    else {
        // Reaching to this branch means
        // application render target has been removed.
        // And it's regarded pragma error.
        assert(false);
    }
}

void Graphics::initGFXComponents() {
    auto desc = GFXComponentsDesc();

    UINT swapCreateFlags = 0u;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // create following stuffs
    // - device (represented by GFXFactory)
    // - swap chain with front/back buffers (represented by GFXSwapChain)
    // - rendering context (represented by GFXPipeline)

    GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
        /* pAdapter = */ nullptr,
        /* DriverType = */ D3D_DRIVER_TYPE_HARDWARE,
        /* Software = */ nullptr,
        /* Flags = */ swapCreateFlags,
        /* pFeatureLevels = */ nullptr,
        /* FeatureLevels = */ 0,
        /* SDKVersion = */ D3D11_SDK_VERSION,
        /* pSwapChainDesc = */ &desc,
        /* ppSwapChain = */ &swapchain_,
        /* ppDevice = */ &factory_,
        /* pFeatureLevel = */ nullptr,
        /* ppImmediateContext = */ &pipeline_
    ));
}

void Graphics::constructAppRenderTarget() {
    wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
    GFX_THROW_FAILED(
        swapchain_.swapchain()->GetBuffer(
            0, __uuidof(ID3D11Resource), &pBackBuffer
        )
    );

    // create depth stencil state
    auto dsStateDesc = D3D11_DEPTH_STENCIL_DESC{
        .DepthEnable = true,
        .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
        .DepthFunc = D3D11_COMPARISON_LESS
    };

    auto pDSState = wrl::ComPtr<ID3D11DepthStencilState>();

    GFX_THROW_FAILED(
        factory_.device()->CreateDepthStencilState( &dsStateDesc, &pDSState )
    );

    // bind depth stencil state
    pipeline_.context()->OMSetDepthStencilState( pDSState.Get(), 1u );

    // create depth stencil texture
    auto client = wnd_.client();
    auto dsDesc = D3D11_TEXTURE2D_DESC{
        .Width = static_cast<UINT>(client.width),
        .Height = static_cast<UINT>(client.height),
        .MipLevels = 1u,
        .ArraySize = 1u,
        .Format = DXGI_FORMAT_D32_FLOAT,
        .SampleDesc = DXGI_SAMPLE_DESC{
            .Count = 1u,
            .Quality = 0u
        },
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
        .CPUAccessFlags = 0u,
        .MiscFlags = 0u
    };

    auto pDepthStencil = wrl::ComPtr<ID3D11Texture2D>();

    GFX_THROW_FAILED(
        factory_.device()->CreateTexture2D(
            &dsDesc, nullptr, &pDepthStencil
        )
    );

    IDAppRenderTarget_ = storage_.load<RenderTarget>(
        factory_, pBackBuffer.Get(), pDepthStencil.Get(),
        D3D11_DEPTH_STENCIL_VIEW_DESC{
            .Format = DXGI_FORMAT_D32_FLOAT,
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
            .Texture2D = D3D11_TEX2D_DSV{
                .MipSlice = 0u
            }
        }
    );

    pipeline_.bind( storage_.get(IDAppRenderTarget_).value() );
}