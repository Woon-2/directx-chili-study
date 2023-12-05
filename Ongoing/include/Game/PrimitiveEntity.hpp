#ifndef __PrimitiveEntity
#define __PrimitiveEntity

#include "Entity.hpp"
#include "Scene.hpp"
#include "RenderDesc.hpp"
#include "GTransformComponent.hpp"
#include "InputComponent.hpp"
#include "InputSystem.hpp"

#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/GraphicsStorage.hpp"
#include "GFX/Core/Pipeline.hpp"
#include "GFX/PipelineObjects/IA.hpp"

#include "GFX/PipelineObjects/Topology.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/PipelineObjects/Shader.hpp"
#include "GFX/PipelineObjects/Viewport.hpp"

#include <d3d11.h>
#include <DirectXMath.h>
#include "GFX/Core/GraphicsNamespaces.hpp"

#include <vector>
#include <memory>

#include "ShaderPath.h"

struct PEFaceColorData {
    GFXColor faceColors[6];
};

class PETopology : public Topology {
public:
    PETopology()
        : Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}
};

class PETransformCBuf : public VSCBuffer<dx::XMMATRIX>{
public:
    PETransformCBuf() = default;
    PETransformCBuf(GFXFactory factory)
        : VSCBuffer<dx::XMMATRIX>(factory, 0u, D3D11_USAGE_DYNAMIC,
            D3D11_CPU_ACCESS_WRITE, initialTransforms()
        ) {}

private:
    static std::vector<dx::XMMATRIX> initialTransforms();
};

class PEColorCBuf : public PSCBuffer<PEFaceColorData> {
public:
    PEColorCBuf() = default;
    PEColorCBuf(GFXFactory factory)
        : PSCBuffer<PEFaceColorData>(
            factory, 0u, D3D11_USAGE_DEFAULT, 0u, initialColors()
        ) {}

private:
    static std::vector<PEFaceColorData> initialColors();
};

class PEVertexShader : public VertexShader {
public:
    PEVertexShader() = default;
    PEVertexShader(GFXFactory factory)
        : VertexShader(factory, inputElemDescs(),
            compiledShaderPath/L"VertexShader.cso"
        ) {}

private:
    static std::vector<D3D11_INPUT_ELEMENT_DESC> inputElemDescs();
};

class PEPixelShader : public PixelShader {
public:
    PEPixelShader() = default;
    PEPixelShader(GFXFactory factory)
        : PixelShader( factory, compiledShaderPath/L"PixelShader.cso" ) {}

private:
};

class PEViewport : public Viewport {
public:
    PEViewport(const Win32::Client& client)
        : Viewport( D3D11_VIEWPORT{
            .TopLeftX = 0.f,
            .TopLeftY = 0.f,
            .Width = static_cast<FLOAT>(client.width),
            .Height = static_cast<FLOAT>(client.height),
            .MinDepth = 0.f,
            .MaxDepth = 1.f
        }) {}
};

class PEDrawContext : public DrawContextIndexed {
public:
    PEDrawContext( UINT numIndex, UINT startIndexLocation,
        INT baseVertexLocation, GFXStorage& mappedStorage,
        GFXStorage::ID IDTransCBuf
    ) : DrawContextIndexed(numIndex, startIndexLocation, baseVertexLocation),
        trans_(), mappedStorage_(&mappedStorage), IDTransCBuf_(IDTransCBuf) {}

    void update(const Transform trans) {
        trans_ = trans;
    }

private:
    void drawCall(GFXPipeline& pipeline) const override;

    Transform trans_;
    GFXStorage* mappedStorage_;
    GFXStorage::ID IDTransCBuf_;
};

template <class T, class VertexBufferT, class IndexBufferT>
class PEDrawComponent : public IDrawComponent {
public:
    using MyType = DrawComponent<T>;
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;
    using MyColor = GFXColor;
    using MyConstantBufferColor = PEFaceColorData;
    using MyVertexBuffer = VertexBufferT;
    using MyIndexBuffer = IndexBufferT;
    using MyTopology = PETopology;
    using MyTransformCBuf = PETransformCBuf;
    using MyColorBuffer = PEColorCBuf;
    using MyVertexShader = PEVertexShader;
    using MyPixelShader = PEPixelShader;
    using MyViewport = PEViewport;
    using MyDrawContext = PEDrawContext;

    PEDrawComponent( GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd
    ) : pipeline_(pipeline),
        pScene_(&scene),
        IDVertexShader_( scene.storage().cache<MyVertexShader>( factory ) ),
        IDPixelShader_( scene.storage().cache<MyPixelShader>( factory ) ),
        IDVertexBuffer_( scene.storage().cache<MyVertexBuffer>( factory ) ),
        IDIndexBuffer_( scene.storage().cache<MyIndexBuffer>( factory ) ),
        IDTopology_( scene.storage().cache<MyTopology>() ),
        IDViewport_( scene.storage().cache<MyViewport>( wnd.client() ) ),
        IDTransformCBuf_( scene.storage().cache<MyTransformCBuf>( factory ) ),
        IDColor_( scene.storage().cache<MyColorBuffer>( factory ) ),
        drawContext_( static_cast<UINT>( MyIndexBuffer::size() ), 0u, 0,
            scene.storage(), IDTransformCBuf_
        ) {}

    void update(const Transform trans) {
        drawContext_.update(trans);
    }

    const RenderDesc renderDesc() const override {
        return RenderDesc{
            .header = {
                .typeID = typeid(T),
                .IDVertexShader = IDVertexShader_,
                .IDPixelShader = IDPixelShader_,
                .IDBuffer = IDVertexBuffer_
            },
            .IDs = { IDVertexShader_, IDPixelShader_,
                IDVertexBuffer_, IDIndexBuffer_, IDTopology_,
                IDViewport_, IDTransformCBuf_, IDColor_
            }
        };
    }

    const IDrawContext* drawContext() const override {
        return &drawContext_;
    }

    IDrawContext* drawContext() override {
        return &drawContext_;
    }

private:
    GFXPipeline pipeline_;
    Scene* pScene_;
    GFXStorage::ID IDVertexShader_;
    GFXStorage::ID IDPixelShader_;
    GFXStorage::ID IDVertexBuffer_;
    GFXStorage::ID IDIndexBuffer_;
    GFXStorage::ID IDTopology_;
    GFXStorage::ID IDViewport_;
    GFXStorage::ID IDTransformCBuf_;
    GFXStorage::ID IDColor_;
    // draw context is here to guarantee
    // it is initalized at the last.
    // since draw context may use other member data,
    // to protect the program from accessing unitialized data,
    // sacrifice memory efficiency.
    MyDrawContext drawContext_;
};

template <class T>
class PrimitiveEntity : public IEntity {
public:
    friend class Loader<PrimitiveEntity>;

    PrimitiveEntity()
        : dc_(), tc_(), ic_() {}

    void update(milliseconds elapsed) override {
        tc_->update(elapsed);
        tc_->setTotal( dx::XMMatrixTranspose(
            (tc_->local() * tc_->global()).get()
        ) );
        dc_->update(tc_->total());
    }

    // ct stands for construct
    void ctDrawComponent(GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd
    ) {
        dc_.reset( new DrawComponent<T>(factory, pipeline, scene, wnd) );
    }

    void ctInputComponent(const MousePointConverter& converter,
        const AppMousePoint& initialPos
    ) {
        ic_.reset( new MouseInputComponent<T>(converter, initialPos) );
    }

    void ctTransformComponent( Distribution& distRadius,
        Distribution& distCTP,  // chi, theta, phi
        Distribution& distDeltaCTP, // chi, theta, phi
        Distribution& distDeltaRTY   // roll, yaw, pitch
    ) {
        tc_.reset( new GTransformComponent(
            distRadius, distCTP, distDeltaCTP, distDeltaRTY
        ) );
    }

    Loader<PrimitiveEntity> loader() const noexcept;

private:
    std::shared_ptr< DrawComponent<T> >
    drawComponent() const noexcept {
        return dc_;
    }

    std::shared_ptr<BasicTransformComponent>
    transformComponent() const noexcept {
        return tc_;
    }

    std::shared_ptr< DrawComponent<T> > dc_;
    std::shared_ptr< GTransformComponent > tc_;
    std::shared_ptr< MouseInputComponent<T> > ic_;
};

template<class T>
class Loader< PrimitiveEntity<T> > {
public:
    Loader(const PrimitiveEntity<T>& entity)
        : entity_(entity) {}

    Loader(const Loader&) = delete;
    Loader& operator=(const Loader&) = delete;
    Loader(Loader&&) = delete;
    Loader& operator=(Loader&&) = delete;

    void loadAt(Scene& scene) {
        scene.addDrawComponent(entity_.drawComponent());
    }

private:
    const PrimitiveEntity<T>& entity_;
};

template <class T>
Loader<PrimitiveEntity<T>> PrimitiveEntity<T>::loader() const noexcept {
    return Loader<PrimitiveEntity<T>>(*this);
}

#endif  // __PrimitiveEntity