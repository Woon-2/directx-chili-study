#ifndef __DrawComponent
#define __DrawComponent

#include "Buffer.hpp"
#include "GraphicsStorage.hpp"

#include "GraphicsNamespaces.hpp"
#include "GraphicsException.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>

#include "ShaderPath.h"

class DrawComponentBase {
public:
    DrawComponentBase() = default;

    DrawComponentBase(wrl::ComPtr<ID3D11Device> pDevice,
        GFXPipeline& pipeline
    ) : pDevice_(pDevice), pPipeline_(pipeline.address()) {

    }

    void render(float angle, float x, float y) {
        decltype(auto) pipeline = *pPipeline_;

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

        auto vertexBufferID = storage_.load<VertexBuffer<Vertex>>(
            device(), std::move(vertices)
        );

        pipeline.bind( storage_.get(vertexBufferID).get() );

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
            context()->IASetIndexBuffer(
                pIndexBuffer.Get(),
                DXGI_FORMAT_R16_UINT,
                0u
            )
        );

        // Set primitive topology to triangle list
        context()->IASetPrimitiveTopology(
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
        context()->VSSetShader( pVertexShader.Get(), 0, 0 );

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
            context()->VSSetConstantBuffers(
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
            context()->PSSetConstantBuffers(
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
        context()->IASetInputLayout(pInputLayout.Get());

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
        context()->PSSetShader( pPixelShader.Get(), 0, 0 );

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
        context()->RSSetViewports( 1u, &vp );

        GFX_THROW_FAILED_VOID(
            context()->DrawIndexed(
                static_cast<UINT>(std::size(indices)),
                0u, 0u
            )
        );
    }

    void setDevice(wrl::ComPtr<ID3D11Device> pDevice) {
        pDevice_ = pDevice;
    }

    void setContext(wrl::ComPtr<ID3D11DeviceContext> pContext) {
        pPipeline_->setContext(pContext);
    }

    wrl::ComPtr<ID3D11Device> device() const noexcept {
        return pDevice_;
    }

    wrl::ComPtr<ID3D11DeviceContext> context() const noexcept {
        return pPipeline_->context();
    }

private:
    wrl::ComPtr<ID3D11Device> pDevice_;
    GFXPipeline* pPipeline_;
    GFXStorage storage_;
};

template <class T>
class DrawComponent {
public:

private:
    
};

#endif  // __DrawComponent