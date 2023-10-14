#ifndef __Graphics
#define __Graphics

#include "ChiliWindow.hpp"
#include <d3d11.h>

#include "Woon2Exception.hpp"

#define NO_GFX_EXCEPT() NoGfxException(__LINE__, __FILE__)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, hr)

class NoGfxException : public Woon2Exception {
public:
    using Woon2Exception::Woon2Exception;
    const char* type() const noexcept override;
private:
};

class DeviceRemovedException : public Win32::WindowException {
public:
    using Win32::WindowException::WindowException;
    const char* type() const noexcept override;

private:
};

template <class Wnd>
class Graphics {
public:
    using MyWindow = Wnd;

    Graphics(MyWindow& wnd)
        : wnd_(wnd), pDevice_(nullptr), pSwap_(nullptr),
        pContext_(nullptr), pTarget_(nullptr) {
        try {

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
        WND_THROW_FAILED( D3D11CreateDeviceAndSwapChain(
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
        ) );

        // TODO: must use RAII
        ID3D11Resource* pBackBuffer = nullptr;
        WND_THROW_FAILED(
            pSwap_->GetBuffer( 0, __uuidof(ID3D11Resource),
                reinterpret_cast<void**>(&pBackBuffer)
            )
        );
        WND_THROW_FAILED(
            pDevice_->CreateRenderTargetView(
                pBackBuffer,
                nullptr,
                &pTarget_
            )
        );
        pBackBuffer->Release();

        }
        catch (const Win32::WindowException& e) {
            MessageBoxA(nullptr, e.what(), "Window Exception",
                MB_OK | MB_ICONEXCLAMATION);
        }
        catch (const std::exception& e) {
            MessageBoxA(nullptr, e.what(), "Standard Exception",
                MB_OK | MB_ICONEXCLAMATION);
        }
        catch(...) {
            MessageBoxA(nullptr, "no details available",
                "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
        }
    }

    ~Graphics() {
        releaseCOM(pDevice_);
        releaseCOM(pSwap_);
        releaseCOM(pContext_);
        releaseCOM(pTarget_);
    }

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    void render() {
        if ( auto hr = pSwap_->Present(2u, 0u); hr < 0 ) {
            if (hr == DXGI_ERROR_DEVICE_REMOVED) {
                throw GFX_DEVICE_REMOVED_EXCEPT(hr);
            }
            else {
                throw WND_EXCEPT(hr);
            }
        }
    }

    void clear( float r, float g, float b ) {
        const float color[] = { r, g, b, 1.f };
        pContext_->ClearRenderTargetView( pTarget_, color );
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
    ID3D11RenderTargetView* pTarget_;
};

#endif  // __Graphics