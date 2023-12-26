#ifndef __PrimitiveEntity
#define __PrimitiveEntity

#include "Entity.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "RenderObjectDesc.hpp"
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
#include "GFX/Core/GraphicsException.hpp"

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

class PEIndexedColorCBuf : public PSCBuffer<PEFaceColorData> {
public:
    PEIndexedColorCBuf() = default;
    PEIndexedColorCBuf(GFXFactory factory)
        : PSCBuffer<PEFaceColorData>(
            factory, 0u, D3D11_USAGE_DEFAULT, 0u, initialColors()
        ) {}

private:
    static std::vector<PEFaceColorData> initialColors();
};

class PEColorBuffer : public VertexBuffer<GFXColor> {
public:
    PEColorBuffer(GFXFactory factory, std::size_t size)
        : VertexBuffer<GFXColor>( factory, makeRandom(size) ) {}

private:
    static std::vector<GFXColor> makeRandom(std::size_t size);
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

template < class T, class PosBufferT, class IndexBufferT >
class PEDrawComponent : public IDrawComponent {
public:
    using MyType = DrawComponent<T>;
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;
    using MyColor = GFXColor;
    using MyConstantBufferColor = PEFaceColorData;
    using MyPosBuffer = PosBufferT;
    using MyIndexBuffer = IndexBufferT;
    using MyTopology = PETopology;
    using MyTransformCBuf = PETransformCBuf;
    using MyIndexedColorCBuf = PEIndexedColorCBuf;
    using MyBlendedColorBuffer = PEColorBuffer;
    using MyViewport = PEViewport;
    using MyDrawContext = PEDrawContext;

    PEDrawComponent( GFXFactory factory, GFXPipeline pipeline,
        GFXStorage& storage, const ChiliWindow& wnd
    ) : 
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        RODesc_(), pipeline_(pipeline), pStorage_(&storage),
        IDPosBuffer_( storage.cache<MyPosBuffer>(factory) ),
        IDIndexBuffer_( storage.cache<MyIndexBuffer>(factory) ),
        IDTopology_( storage.cache<MyTopology>() ),
        IDViewport_( storage.cache<MyViewport>( wnd.client() ) ),
        IDTransformCBuf_( storage.cache<MyTransformCBuf>( factory ) ),
        IDIndexedColorCBuf_( storage.cache<MyIndexedColorCBuf>( factory ) ),
        IDBlendedColorBuffer_( storage.cache<MyBlendedColorBuffer>(
            factory, MyPosBuffer::size()
        ) ),
        drawContext_(
            static_cast<UINT>( MyIndexBuffer::size() ),
            0u, 0, storage, IDTransformCBuf_
        ) {
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_.entryStackPop();
    #endif
    }

    template <class ... TesselationFactors>
    PEDrawComponent( GFXFactory factory, GFXPipeline pipeline,
        GFXStorage& storage, const ChiliWindow& wnd,
        TesselationFactors&& ... tesselationFactors
    ) :
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        RODesc_(), pipeline_(pipeline), pStorage_(&storage),
        IDPosBuffer_( storage.load<MyPosBuffer>(
            factory, std::forward<TesselationFactors>(tesselationFactors)...
        ) ),
        IDIndexBuffer_( storage.load<MyIndexBuffer>(
            factory, std::forward<TesselationFactors>(tesselationFactors)...
        ) ),
        IDTopology_( storage.cache<MyTopology>() ),
        IDViewport_( storage.cache<MyViewport>( wnd.client() ) ),
        IDTransformCBuf_( storage.cache<MyTransformCBuf>( factory ) ),
        IDIndexedColorCBuf_( storage.cache<MyIndexedColorCBuf>( factory ) ),
        IDBlendedColorBuffer_( storage.load<MyBlendedColorBuffer>(
            factory, MyPosBuffer::size(
                std::forward<TesselationFactors>(tesselationFactors)...
            )
        ) ),
        drawContext_(
            static_cast<UINT>( MyIndexBuffer::size(
                std::forward<TesselationFactors>(tesselationFactors)...
            ) ),
            0u, 0, storage, IDTransformCBuf_
        ) {
        #ifdef ACTIVATE_DRAWCOMPONENT_LOG
            logComponent_.entryStackPop();
        #endif
        }

    void update(const Transform trans) {
        drawContext_.update(trans);
    }

    const RenderObjectDesc renderObjectDesc() const override {
        if (!RODesc_.has_value()) {
            throw GFX_EXCEPT_CUSTOM("DrawComponent is not synchronized with any renderer.\n");
        }
        return RODesc_.value();
    }

    void sync(const Renderer& renderer) override {
        if ( typeid(renderer) == typeid(IndexedRenderer) ) {
            sync( static_cast<const IndexedRenderer&>(renderer) );
        }
        else if ( typeid(renderer) == typeid(BlendedRenderer) ) {
            sync( static_cast<const BlendedRenderer&>(renderer) );
        }
        else {
            throw GFX_EXCEPT_CUSTOM("DrawComponent tried to synchronize with incompatible renderer.\n");
        }
    }
    
    void sync(const IndexedRenderer& renderer) {
        assert(pStorage_->get(IDPosBuffer_).has_value());
        static_cast<MyPosBuffer*>( pStorage_->get(IDPosBuffer_).value() )
            ->setSlot( IndexedRenderer::slotPosBuffer() );

        RODesc_ = RenderObjectDesc{
            .header = {
                .IDBuffer = IDPosBuffer_,
                .IDType = typeid(T)
            },
            .IDs = {
                IDPosBuffer_, IDIndexBuffer_, IDTopology_,
                IDViewport_, IDTransformCBuf_, IDIndexedColorCBuf_
            }
        };
    }

    void sync(const BlendedRenderer& renderer) {
        assert(pStorage_->get(IDPosBuffer_).has_value());
        static_cast<MyPosBuffer*>( pStorage_->get(IDPosBuffer_).value() )
            ->setSlot( BlendedRenderer::slotPosBuffer() );
        static_cast<MyBlendedColorBuffer*>(
            pStorage_->get(IDBlendedColorBuffer_).value()
        )->setSlot( BlendedRenderer::slotColorBuffer() );

        RODesc_ = RenderObjectDesc{
            .header = {
                .IDBuffer = IDPosBuffer_,
                .IDType = typeid(T)
            },
            .IDs = {
                IDPosBuffer_, IDBlendedColorBuffer_, IDIndexBuffer_, IDTopology_,
                IDViewport_, IDTransformCBuf_
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
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
    IDrawComponent::LogComponent logComponent_;
#endif
    std::optional<RenderObjectDesc> RODesc_;
    GFXPipeline pipeline_;
    GFXStorage* pStorage_;
    GFXStorage::ID IDPosBuffer_;
    GFXStorage::ID IDIndexBuffer_;
    GFXStorage::ID IDTopology_;
    GFXStorage::ID IDViewport_;
    GFXStorage::ID IDTransformCBuf_;
    GFXStorage::ID IDIndexedColorCBuf_;
    GFXStorage::ID IDBlendedColorBuffer_;
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
    template <class ... TesselationFactors>
    void ctDrawComponent(GFXFactory factory, GFXPipeline pipeline,
        GFXStorage& storage, const ChiliWindow& wnd,
        TesselationFactors&& ... tesselationFactors
    ) {
        dc_.reset( new DrawComponent<T>(factory, pipeline, storage, wnd,
            std::forward<TesselationFactors>(tesselationFactors)...
        ) );
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