#include "Game/Box.hpp"

#include "GFX/Core/GraphicsStorage.hpp"
#include "GFX/PipelineObjects/Bindable.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/PipelineObjects/Shader.hpp"
#include "GFX/PipelineObjects/Topology.hpp"
#include "GFX/PipelineObjects/Viewport.hpp"

#include "ShaderPath.h"

#include <cassert>

class DrawComponent<Box>::MyVertexBuffer : public VertexBuffer<MyVertex> {
public:
    MyVertexBuffer() = default;
    MyVertexBuffer(GFXFactory factory)
        : VertexBuffer<MyVertex>( factory, modelVertices() ) {}

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

class DrawComponent<Box>::MyIndexBuffer : public IndexBuffer<MyIndex> {
public:
    MyIndexBuffer() = default;
    MyIndexBuffer(GFXFactory factory)
        : IndexBuffer<MyIndex>( factory, modelIndices() ) {}

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

class DrawComponent<Box>::MyTopology : public Topology {
public:
    MyTopology()
        : Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}
};

class DrawComponent<Box>::MyTransform : public VSCBuffer<dx::XMMATRIX> {
public:
    MyTransform() = default;
    MyTransform(GFXFactory factory)
        : VSCBuffer<dx::XMMATRIX>(factory, 0u, D3D11_USAGE_DYNAMIC,
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

class DrawComponent<Box>::MyColorBuffer : public PSCBuffer<MyConstantBufferColor> {
public:
    MyColorBuffer() = default;
    MyColorBuffer(GFXFactory factory)
        : PSCBuffer<MyConstantBufferColor>(
            factory, 0u, D3D11_USAGE_DEFAULT, 0u, initialColors()
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

class DrawComponent<Box>::MyVertexShader : public VertexShader {
public:
    MyVertexShader() = default;
    MyVertexShader(GFXFactory factory)
        : VertexShader(factory, inputElemDescs(),
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

class DrawComponent<Box>::MyPixelShader : public PixelShader {
public:
    MyPixelShader() = default;
    MyPixelShader(GFXFactory factory)
        : PixelShader( factory, compiledShaderPath/L"PixelShader.cso" ) {}

private:
};

class DrawComponent<Box>::MyViewport : public Viewport {
public:
    MyViewport(const Win32::Client& client)
        : Viewport( D3D11_VIEWPORT{
            .TopLeftX = 0.f,
            .TopLeftY = 0.f,
            .Width = static_cast<FLOAT>(client.width),
            .Height = static_cast<FLOAT>(client.height),
            .MinDepth = 0.f,
            .MaxDepth = 1.f
        }) {}
};

DrawComponent<Box>::DrawComponent( GFXFactory factory, GFXPipeline pipeline,
    Scene& scene, const ChiliWindow& wnd 
) : drawContext_( static_cast<UINT>( MyIndexBuffer::size() ), 0u, 0 ),
    pipeline_(pipeline),
    pScene_(&scene),
    IDVertexShader_( scene.storage().cache<MyVertexShader>( factory ) ),
    IDPixelShader_( scene.storage().cache<MyPixelShader>( factory ) ),
    IDVertexBuffer_( scene.storage().cache<MyVertexBuffer>( factory ) ),
    IDIndexBuffer_( scene.storage().cache<MyIndexBuffer>( factory ) ),
    IDTopology_( scene.storage().cache<MyTopology>() ),
    IDViewport_( scene.storage().cache<MyViewport>( wnd.client() ) ),
    IDTransform_( scene.storage().cache<MyTransform>( factory ) ),
    IDColor_( scene.storage().cache<MyColorBuffer>( factory ) ) {}

void DrawComponent<Box>::update(const Transform trans) {
    assert( pScene_->storage().get(IDTransform_).has_value() );

    auto transCBuf = static_cast<MyTransform*>(
        pScene_->storage().get(IDTransform_).value()
    );

    transCBuf->dynamicUpdate(pipeline_, [trans](){ return trans.data(); });
}

void MouseInputComponent<Box>::receive(const Mouse::Event& ev) {
    if (ev.moved()) {
        curPos_ = pConverter_->convert( ev.pos() );
    }
}

void MouseInputComponent<Box>::update() {
    lastPos_ = curPos_;
}

void Entity<Box>::update(milliseconds elapsed) {
    auto mousePos = ic_->pos();
    tc_->setTotal(
        dx::XMMatrixTranspose(
            tc_->local().get()
            * dx::XMMatrixRotationY(
                std::chrono::duration_cast<seconds>(elapsed).count()
            )
            * dx::XMMatrixRotationX(
                std::chrono::duration_cast<seconds>(elapsed).count()
            )
            * dx::XMMatrixTranslation( 0.f, 0.f, 4.f )
            * dx::XMMatrixPerspectiveLH( 1.f, 3.f/4.f, 0.5f, 10.f )
            * dx::XMMatrixTranslation( mousePos.x, mousePos.y, 0.f )
        )
    );
    dc_->update(tc_->total());
}

void Entity<Box>::ctDrawComponent(GFXFactory factory, GFXPipeline pipeline,
    Scene& scene, const ChiliWindow& wnd
) {
    dc_.reset( new DrawComponent<Box>(factory, pipeline, scene, wnd) );
}

void Entity<Box>::ctInputComponent( const MousePointConverter& converter,
    const AppMousePoint& initialPos
) {
    ic_.reset( new MouseInputComponent<Box>(converter, initialPos) );
}

void Entity<Box>::ctTransformComponent() {
    tc_.reset( new BasicTransformComponent() );
}

Loader<Box> Entity<Box>::loader() const noexcept {
    return Loader<Box>(*this);
}