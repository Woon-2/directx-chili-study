#ifndef __Box
#define __Box

#include "Entity.hpp"
#include "Scene.hpp"
#include "RenderDesc.hpp"
#include "DrawComponent.hpp"
#include "GraphicsStorage.hpp"

#include "Bindable.hpp"
#include "Pipeline.hpp"
#include "IA.hpp"
#include "Buffer.hpp"
#include "Shader.hpp"
#include "Topology.hpp"
#include "Viewport.hpp"

#include <vector>

#include "GraphicsNamespaces.hpp"

class Box {

};

template <>
class DrawComponent<Box> : public IDrawComponent {
public:
    using MyType = DrawComponent<Box>;
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;

    class MyVertexBuffer : public VertexBuffer<MyVertex> {
    public:
        MyVertexBuffer() = default;
        MyVertexBuffer(wrl::ComPtr<ID3D11Device> pDevice)
            : VertexBuffer<MyVertex>( pDevice, modelVertices() ) {}

    private:
        static constexpr std::vector<MyVertex> modelVertices() {
            return std::vector<MyVertex>{
                {-1.f, -1.f, -1.f},
                {1.f, -1.f, -1.f},
                {-1.f, 1.f, -1.f},
                {1.f, 1.f, -1.f},
                {-1.f, -1.f, 1.f},
                {1.f, -1.f, 1.f},
                {-1.f, 1.f, 1.f},           
                {1.f, 1.f, 1.f}
            };
        }
    };

    class MyIndexBuffer : public IndexBuffer<MyIndex> {
    public:
        MyIndexBuffer() = default;
        MyIndexBuffer(wrl::ComPtr<ID3D11Device> pDevice)
            : IndexBuffer<MyIndex>( pDevice, modelIndices() ) {}

        static constexpr std::size_t size() {
            return 36u;
        }

    private:
        static constexpr std::vector<MyIndex> modelIndices() {
            return std::vector<MyIndex>{
                0, 2, 1, 2, 3, 1,
                1, 3, 5, 3, 7, 5,
                2, 6, 3, 3, 6, 7,
                4, 5, 7, 4, 7, 6,
                0, 4, 2, 2, 4, 6,
                0, 1, 4, 1, 5, 4
            };
        }
    };

    class MyTopology : public Topology {
    public:
        MyTopology()
            : Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}
    };

    class MyTransform : public VSCBuffer<dx::XMMATRIX> {
    public:
        MyTransform() = default;
        MyTransform(wrl::ComPtr<ID3D11Device> pDevice)
            : VSCBuffer<dx::XMMATRIX>(pDevice, 0u, D3D11_USAGE_DYNAMIC,
                D3D11_CPU_ACCESS_WRITE, initialTransforms()
            ) {}

    private:
        static constexpr std::vector<dx::XMMATRIX> initialTransforms() {
            return std::vector<dx::XMMATRIX>{
                dx::XMMatrixTranspose(
                    dx::XMMatrixIdentity()
                    * dx::XMMatrixRotationY(0.7f)
                    * dx::XMMatrixRotationX(-0.4f)
                    * dx::XMMatrixTranslation( 0.f, 0.f, 4.f )
                    * dx::XMMatrixPerspectiveLH( 1.f, 3.f/4.f, 0.5f, 10.f )
                )
            };
        }
    };

    struct MyColor {
        float r;
        float g;
        float b;
        float a;
    };

    struct MyConstantBufferColor {
        MyColor faceColors[6];
    };

    class MyColorBuffer : public PSCBuffer<MyConstantBufferColor> {
    public:
        MyColorBuffer() = default;
        MyColorBuffer(wrl::ComPtr<ID3D11Device> pDevice)
            : PSCBuffer<MyConstantBufferColor>(
                pDevice, 0u, D3D11_USAGE_DEFAULT, 0u, initialColors()
            ) {}

    private:
        static constexpr std::vector<MyConstantBufferColor> initialColors() {
            return std::vector<MyConstantBufferColor>{{
                MyColor{1.f, 0.f, 1.f, 1.f},
                MyColor{1.f, 0.f, 0.f, 1.f},
                MyColor{0.f, 1.f, 0.f, 1.f},
                MyColor{0.f, 0.f, 1.f, 1.f},
                MyColor{1.f, 1.f, 0.f, 1.f},
                MyColor{0.f, 1.f, 1.f, 1.f}
            }};
        }
    };

    class MyVertexShader : public VertexShader {
    public:
        MyVertexShader() = default;
        MyVertexShader(wrl::ComPtr<ID3D11Device> pDevice)
            : VertexShader(pDevice, inputElemDescs(),
                compiledShaderPath/L"VertexShader.cso"
            ) {}

    private:
        static constexpr std::vector<D3D11_INPUT_ELEMENT_DESC>
            inputElemDescs() {
            return std::vector<D3D11_INPUT_ELEMENT_DESC>{
                { .SemanticName = "Position",
                  .SemanticIndex = 0,
                  .Format = DXGI_FORMAT_R32G32B32_FLOAT,
                  .InputSlot = 0,
                  .AlignedByteOffset = 0,
                  .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
                  .InstanceDataStepRate = 0
                }
            };
        }
    };

    class MyPixelShader : public PixelShader {
    public:
        MyPixelShader() = default;
        MyPixelShader(wrl::ComPtr<ID3D11Device> pDevice)
            : PixelShader( pDevice, compiledShaderPath/L"PixelShader.cso" ) {}

    private:
    };

    class MyViewport : public Viewport {
    public:
        MyViewport()
            : Viewport( D3D11_VIEWPORT{
                .TopLeftX = 0,
                .TopLeftY = 0,
                .Width = 800,
                .Height = 600,
                .MinDepth = 0,
                .MaxDepth = 1
            }) {}
    };

    const RenderDesc renderDesc() const override {
        // assert(vertexShaderID_.has_value());
        // assert(pixelShaderID_.has_value());
        return RenderDesc{
            .header = {
                .typeID = typeid(Box),
                .vertexShaderID = IDVertexShader_,
                .pixelShaderID = IDPixelShader_
            },
            .IDs = { IDVertexShader_, IDPixelShader_,
                IDVertexBuffer_, IDIndexBuffer_, IDTopology_,
                IDViewport_, IDTransform_, IDColor_
            }
        };
    }

    const IDrawContext* drawContext() const override {
        return &drawContext_;
    }

    IDrawContext* drawContext() override {
        return &drawContext_;
    }

    static void loadAt( wrl::ComPtr<ID3D11Device> pDevice,
        Box& box, Scene& scene
    ) {
        scene.addDrawComponent( new MyType(pDevice, box, scene) );
    }

private:
    DrawComponent( wrl::ComPtr<ID3D11Device> pDevice,
        Box& box, Scene& scene
    ) : drawContext_( static_cast<UINT>( MyIndexBuffer::size() ), 0u, 0 ),
        IDVertexShader_( scene.storage().cache<MyVertexShader>( pDevice ) ),
        IDPixelShader_( scene.storage().cache<MyPixelShader>( pDevice ) ),
        IDVertexBuffer_( scene.storage().cache<MyVertexBuffer>( pDevice ) ),
        IDIndexBuffer_( scene.storage().cache<MyIndexBuffer>( pDevice ) ),
        IDTopology_( scene.storage().cache<MyTopology>() ),
        IDViewport_( scene.storage().cache<MyViewport>() ),
        IDTransform_( scene.storage().load<MyTransform>( pDevice ) ),
        IDColor_( scene.storage().load<MyColorBuffer>( pDevice ) ) {}

    DrawContextIndexed drawContext_;
    GFXStorage::ID IDVertexShader_;
    GFXStorage::ID IDPixelShader_;
    GFXStorage::ID IDVertexBuffer_;
    GFXStorage::ID IDIndexBuffer_;
    GFXStorage::ID IDTopology_;
    GFXStorage::ID IDViewport_;
    GFXStorage::ID IDTransform_;
    GFXStorage::ID IDColor_;
};

template<>
class Loader<Box> {
public:
    Loader(Box& box) : box_(box) {}

    void loadAt(wrl::ComPtr<ID3D11Device> pDevice, Scene& scene) {
        DrawComponent<Box>::loadAt(pDevice, box_, scene);
    }

private:
    Box box_;
};

#endif