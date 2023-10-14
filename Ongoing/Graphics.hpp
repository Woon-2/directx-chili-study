#ifndef __Graphics
#define __Graphics

#include "ChiliWindow.hpp"
#include <d3d11.h>

template <class Wnd>
class Graphics {
public:
    using MyWindow = Wnd;

    Graphics(MyWindow& wnd)
        : wnd_(wnd) {
        auto sd = DXGI_SWAP_CHAIN_DESC{
            .BufferDesc = DXGI_MODE_DESC{
                .Width = 0,
                .Height = 0,
                .RefreshRate = DXGI_RATIONAL{
                    .Numerator = 0,
                    .Denominator = 0
                },
                .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
                .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
                .Scaling = DXGI_MODE_SCALING_UNSPECIFIED
            },
            .SampleDesc = DXGI_SAMPLE_DESC {
                .Count = 1,
                .Quality = 0
            },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 1,
            .OutputWindow = wnd_.nativeHandle(),
            .Windowed = true,
            .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
            .Flags = 0
        };

        // create device and front/back buffers, swap chain, and rendering context
        D3D11CreateDeviceAndSwapChain(
            /* pAdapter = */ nullptr,
            /* DriverType = */ D3D_DRIVER_TYPE_HARDWARE,
            /* Software = */ nullptr,
            /* Flags = */ 0,
            /* pFeatureLevels = */ nullptr,
            /* FeatureLevels = */ 0,
            /* SDKVersion = */ D3D11_SDK_VERSION,
            /* pSwapChainDesc = */ &sd,
            /* ppSwapChain = */ &pSwap_,
            /* ppDevice = */ &pDevice_,
            /* pFeatureLevel = */ nullptr,
            /* ppImmediateContext = */ &pContext_
        );
    }

    ~Graphics() {
        releaseCOM(pDevice_);
        releaseCOM(pSwap_);
        releaseCOM(pContext_);
    }

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    void render() {
        pSwap_->Present(2u, 0u);
    }

private:
    static void releaseCOM(IUnknown* com) {
        if (com) {
            com->Release();
            com = nullptr;
        }
    }

    MyWindow& wnd_;
    ID3D11Device* pDevice_;
    IDXGISwapChain* pSwap_;
    ID3D11DeviceContext* pContext_;
};

#endif  // __Graphics