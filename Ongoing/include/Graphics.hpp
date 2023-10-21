#ifndef __Graphics
#define __Graphics

#include "ChiliWindow.hpp"
#include "DrawComponent.hpp"
#include "Bindable.hpp"

#include "GraphicsNamespaces.hpp"
#include "GraphicsException.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>

#include <vector>
#include <string>
#include <cstddef>
#include <cmath>
#include <unordered_set>
#include <memory>

#include "ShaderPath.h"

class GFXPipeline {
public:
    GFXPipeline() = default;

    GFXPipeline(wrl::ComPtr<ID3D11DeviceContext> pContext)
        : pContext_(pContext) {

    }

    void bind(IBindable* bindable) {
        bindable->bind(*this);
    }

    void setContext(wrl::ComPtr<ID3D11DeviceContext> pContext) {
        pContext_ = pContext;
    }

    wrl::ComPtr<ID3D11DeviceContext> context() const noexcept {
        return pContext_;
    }

private:
    wrl::ComPtr<ID3D11DeviceContext> pContext_;
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

            UINT swapCreateFlags = 0u;
#ifndef NDEBUG
            swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

            // create device and front/back buffers, swap chain, and rendering context
            GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
                /* pAdapter = */ nullptr,
                /* DriverType = */ D3D_DRIVER_TYPE_HARDWARE,
                /* Software = */ nullptr,
                /* Flags = */ swapCreateFlags,
                /* pFeatureLevels = */ nullptr,
                /* FeatureLevels = */ 0,
                /* SDKVersion = */ D3D11_SDK_VERSION,
                /* pSwapChainDesc = */ &sd,
                /* ppSwapChain = */ &pSwap_,
                /* ppDevice = */ &pDevice_,
                /* pFeatureLevel = */ nullptr,
                /* ppImmediateContext = */ &pContext_
            ));

            wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
            GFX_THROW_FAILED(
                pSwap_->GetBuffer(
                    0, __uuidof(ID3D11Resource), &pBackBuffer
                )
            );
            GFX_THROW_FAILED(
                pDevice_->CreateRenderTargetView(
                    pBackBuffer.Get(),
                    nullptr,
                    &pTarget_
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
                pDevice_->CreateDepthStencilState( &dsStateDesc, &pDSState )
            );

            // bind depth stencil state
            pContext_->OMSetDepthStencilState( pDSState.Get(), 1u );

            // create depth stencil texture
            auto dsDesc = D3D11_TEXTURE2D_DESC{
                .Width = 800u,
                .Height = 600u,
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
                pDevice_->CreateTexture2D(
                    &dsDesc, nullptr, &pDepthStencil
                )
            );

            // create view of depth stencil texture
            auto desvDesc = D3D11_DEPTH_STENCIL_VIEW_DESC{
                .Format = DXGI_FORMAT_D32_FLOAT,
                .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
                .Texture2D = D3D11_TEX2D_DSV{
                    .MipSlice = 0u
                }
            };
            GFX_THROW_FAILED(
                pDevice_->CreateDepthStencilView(
                    pDepthStencil.Get(), &desvDesc, &pDSV_
                )
            );

            // bind depth stencil view to output merger
            pContext_->OMSetRenderTargets(
                1u, pTarget_.GetAddressOf(), pDSV_.Get()
            );
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

    ~Graphics() = default;

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    void render(float angle, float x, float y) {
        drawTriangle(angle, x, y);
        drawTriangle(-angle, 0.f, 0.f);

        if (auto hr = pSwap_->Present(2u, 0u); hr < 0) {
            if (hr == DXGI_ERROR_DEVICE_REMOVED) {
                throw GFX_DEVICE_REMOVED_EXCEPT(hr);
            }
            else {
                throw GFX_EXCEPT(hr);
            }
        }
    }

    void clear(float r, float g, float b) {
        const float color[] = { r, g, b, 1.f };
        pContext_->ClearRenderTargetView(pTarget_.Get(), color);
        pContext_->ClearDepthStencilView(
            pDSV_.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u
        );
    }

    void drawTriangle(float angle, float x, float y) {
        auto dcb = DrawComponentBase(pDevice_, pContext_);
        dcb.render(angle, x, y);
    }

private:
    MyWindow& wnd_;
    wrl::ComPtr<ID3D11Device> pDevice_;
    wrl::ComPtr<IDXGISwapChain> pSwap_;
    wrl::ComPtr<ID3D11DeviceContext> pContext_;
    wrl::ComPtr<ID3D11RenderTargetView> pTarget_;
    wrl::ComPtr<ID3D11DepthStencilView> pDSV_;
};

#endif  // __Graphics