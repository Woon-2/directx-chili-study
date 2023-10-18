#ifndef __Graphics
#define __Graphics

#include "ChiliWindow.hpp"

#include "GraphicsNamespaces.hpp"
#include "GraphicsException.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>

#include <vector>
#include <string>
#include <cstddef>
#include <cmath>

#include "ShaderPath.h"

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
        struct Vertex {
            struct Pos {
                float x;
                float y;
                float z;
            };
            Pos pos;
        };

        // Create Vertex Buffer
        const Vertex vertices[] = {
            {-1.f, -1.f, -1.f},
            {1.f, -1.f, -1.f},
            {-1.f, 1.f, -1.f},
            {1.f, 1.f, -1.f},
            {-1.f, -1.f, 1.f},
            {1.f, -1.f, 1.f},
            {-1.f, 1.f, 1.f},           
            {1.f, 1.f, 1.f}
        };

        auto pVertexBuffer = wrl::ComPtr<ID3D11Buffer>();

        auto vbd = D3D11_BUFFER_DESC{
            .ByteWidth = sizeof(vertices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = 0u,
            .MiscFlags = 0u,
            .StructureByteStride = sizeof(Vertex)
        };

        auto vsd = D3D11_SUBRESOURCE_DATA{
            .pSysMem = vertices
        };

        GFX_THROW_FAILED(
            pDevice_->CreateBuffer(&vbd, &vsd, &pVertexBuffer)
        );

        // Bind Vertex Buffer
        const UINT stride = sizeof(Vertex);
        const UINT offset = 0u;

        GFX_THROW_FAILED_VOID(
            pContext_->IASetVertexBuffers(
                0u, 1u, pVertexBuffer.GetAddressOf(),
                &stride, &offset
            )
        );

        // Create Index Buffer
        const unsigned short indices[] = {
            0, 2, 1, 2, 3, 1,
            1, 3, 5, 3, 7, 5,
            2, 6, 3, 3, 6, 7,
            4, 5, 7, 4, 7, 6,
            0, 4, 2, 2, 4, 6,
            0, 1, 4, 1, 5, 4
        };

        auto pIndexBuffer = wrl::ComPtr<ID3D11Buffer>();

        auto ibd = D3D11_BUFFER_DESC{
            .ByteWidth = sizeof(indices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
            .CPUAccessFlags = 0u,
            .MiscFlags = 0u,
            .StructureByteStride = sizeof(unsigned short)
        };

        auto isd = D3D11_SUBRESOURCE_DATA{
            .pSysMem = indices
        };

        GFX_THROW_FAILED(
            pDevice_->CreateBuffer(&ibd, &isd, &pIndexBuffer)
        );

        // Bind Index Buffer
        GFX_THROW_FAILED_VOID(
            pContext_->IASetIndexBuffer(
                pIndexBuffer.Get(),
                DXGI_FORMAT_R16_UINT,
                0u
            )
        );

        // Set primitive topology to triangle list
        pContext_->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );

        // Create Vertex Shader
        auto pVertexShader = wrl::ComPtr<ID3D11VertexShader>();
        auto pBlob = wrl::ComPtr<ID3DBlob>();
        GFX_THROW_FAILED(
            D3DReadFileToBlob(
                (compiledShaderPath/L"VertexShader.cso").c_str(),
                &pBlob 
            )
        );
        GFX_THROW_FAILED(
            pDevice_->CreateVertexShader(
                pBlob->GetBufferPointer(),
                pBlob->GetBufferSize(),
                nullptr,
                &pVertexShader
            )
        );

        // Bind Vertex Shader
        pContext_->VSSetShader( pVertexShader.Get(), 0, 0 );

        // Create Constant Buffer for Transformation Matrix
        struct ConstantBuffer {
            dx::XMMATRIX transform;
        };
        
        const auto cbuf = ConstantBuffer{
            {
                dx::XMMatrixTranspose(
                    dx::XMMatrixRotationZ( angle )
                    * dx::XMMatrixRotationX( angle )
                    * dx::XMMatrixTranslation( x, 0.f, y + 4.f )
                    * dx::XMMatrixPerspectiveLH( 1.f, 3.f/4.f, 0.5f, 10.f )
                )
            }
        };

        auto pConstantBuffer = wrl::ComPtr<ID3D11Buffer>();

        auto cbd = D3D11_BUFFER_DESC{
            .ByteWidth = sizeof(ConstantBuffer),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
            .MiscFlags = 0u,
            .StructureByteStride = 0u
        };

        auto csd = D3D11_SUBRESOURCE_DATA{
            .pSysMem = &cbuf
        };

        GFX_THROW_FAILED(
            pDevice_->CreateBuffer(&cbd, &csd, &pConstantBuffer)
        );

        // Bind Constant Buffer
        GFX_THROW_FAILED_VOID(
            pContext_->VSSetConstantBuffers(
                0u, 1u, pConstantBuffer.GetAddressOf()
            )
        );

        // Create Constant Buffer for Face Color
        struct ConstantBufferColor {
            struct {
                float r;
                float g;
                float b;
                float a;
            } face_colors[6];
        };

        auto cbufColor = ConstantBufferColor{
            {
                {1.f, 0.f, 1.f, 0.f},
                {1.f, 0.f, 0.f, 0.f},
                {0.f, 1.f, 0.f, 0.f},
                {0.f, 0.f, 1.f, 0.f},
                {1.f, 1.f, 0.f, 0.f},
                {0.f, 1.f, 1.f, 0.f}
            }
        };

        auto cbdColor = D3D11_BUFFER_DESC{
            .ByteWidth = sizeof(ConstantBufferColor),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0u,
            .MiscFlags = 0u,
            .StructureByteStride = 0u
        };

        auto csdColor = D3D11_SUBRESOURCE_DATA{
            .pSysMem = &cbufColor
        };

        auto pConstantBufferColor = wrl::ComPtr<ID3D11Buffer>();

        GFX_THROW_FAILED(
            pDevice_->CreateBuffer(
                &cbdColor, &csdColor, &pConstantBufferColor
            )
        );

        // Bind Constant Buffer
        GFX_THROW_FAILED_VOID(
            pContext_->PSSetConstantBuffers(
                0u, 1u, pConstantBufferColor.GetAddressOf()
            )
        );

        // Layout Vertex Shader Input
        auto pInputLayout = wrl::ComPtr<ID3D11InputLayout>();
        const D3D11_INPUT_ELEMENT_DESC ied[] = {
            { .SemanticName = "Position",
              .SemanticIndex = 0,
              .Format = DXGI_FORMAT_R32G32B32_FLOAT,
              .InputSlot = 0,
              .AlignedByteOffset = 0,
              .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
              .InstanceDataStepRate = 0
            }
        };

        GFX_THROW_FAILED(
            pDevice_->CreateInputLayout(
                ied,
                static_cast<UINT>( std::size(ied) ),
                pBlob->GetBufferPointer(),
                pBlob->GetBufferSize(),
                &pInputLayout
            )
        );

        // Bind Vertex Input Layout
        pContext_->IASetInputLayout(pInputLayout.Get());

        // Create Pixel Shader
        auto pPixelShader = wrl::ComPtr<ID3D11PixelShader>();
        GFX_THROW_FAILED(
            D3DReadFileToBlob(
                (compiledShaderPath/L"PixelShader.cso").c_str(),
                &pBlob
            )
        );
        GFX_THROW_FAILED(
            pDevice_->CreatePixelShader(
                pBlob->GetBufferPointer(),
                pBlob->GetBufferSize(),
                nullptr,
                &pPixelShader
            )
        );

        // Bind Pixel Shader
        pContext_->PSSetShader( pPixelShader.Get(), 0, 0 );

        // Configure Viewport
        auto vp = D3D11_VIEWPORT{
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = 800,
            .Height = 600,
            .MinDepth = 0,
            .MaxDepth = 1
        };

        // Bind Viewport
        pContext_->RSSetViewports( 1u, &vp );

        GFX_THROW_FAILED_VOID(
            pContext_->DrawIndexed(
                static_cast<UINT>(std::size(indices)),
                0u, 0u
            )
        );
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