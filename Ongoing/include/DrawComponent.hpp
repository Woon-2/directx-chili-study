#ifndef __DrawComponent
#define __DrawComponent

#include "Buffer.hpp"
#include "Topology.hpp"
#include "Shader.hpp"
#include "Viewport.hpp"

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

        auto indexBufferID = storage_.load<IndexBuffer<unsigned short>>(
            device(), std::move(indices)
        );

        pipeline.bind( storage_.get(indexBufferID).get() );

        auto topologyID = storage_.load<Topology>(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );

        pipeline.bind( storage_.get(topologyID).get() );

        // Layout Vertex Shader Input
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

        auto vertexShaderID = storage_.load<VertexShader>(
            device(), std::move(ied),
            compiledShaderPath/L"VertexShader.cso"
        );

        pipeline.bind( storage_.get(vertexShaderID).get() );

        auto pBlob = wrl::ComPtr<ID3DBlob>();

        // Create Constant Buffer for Transformation Matrix
        struct Transform {
            dx::XMMATRIX transform;
        };
        
        const Transform cbuf[] = {
            {
                dx::XMMatrixTranspose(
                    dx::XMMatrixRotationZ( angle )
                    * dx::XMMatrixRotationX( angle )
                    * dx::XMMatrixTranslation( x, 0.f, y + 4.f )
                    * dx::XMMatrixPerspectiveLH( 1.f, 3.f/4.f, 0.5f, 10.f )
                )
            }
        };

        auto transformID = storage_.load<VSCBuffer<Transform>>(
            device(), 0u, D3D11_USAGE_DYNAMIC,
            D3D11_CPU_ACCESS_WRITE, std::move(cbuf)
        );

        pipeline.bind( storage_.get(transformID).get() );

        // Create Constant Buffer for Face Color
        struct ConstantBufferColor {
            struct {
                float r;
                float g;
                float b;
                float a;
            } face_colors[6];
        };

        const ConstantBufferColor cbufColor[] = {
            {{
                {1.f, 0.f, 1.f, 0.f},
                {1.f, 0.f, 0.f, 0.f},
                {0.f, 1.f, 0.f, 0.f},
                {0.f, 0.f, 1.f, 0.f},
                {1.f, 1.f, 0.f, 0.f},
                {0.f, 1.f, 1.f, 0.f}
            }}
        };

        auto cbufColorID = storage_.load< PSCBuffer<ConstantBufferColor> >(
            device(), 0u, D3D11_USAGE_DEFAULT,
            0u, std::move(cbufColor)
        );

        pipeline.bind( storage_.get(cbufColorID).get() );

        auto pixelShaderID = storage_.load<PixelShader>(
            device(), compiledShaderPath/L"PixelShader.cso"
        );

        pipeline.bind( storage_.get(pixelShaderID).get() );

        auto viewportID = storage_.load<Viewport>(
            D3D11_VIEWPORT{
                .TopLeftX = 0,
                .TopLeftY = 0,
                .Width = 800,
                .Height = 600,
                .MinDepth = 0,
                .MaxDepth = 1
            }
        );

        pipeline.bind( storage_.get(viewportID).get() );

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