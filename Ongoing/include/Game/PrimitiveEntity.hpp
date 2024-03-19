#ifndef __PrimitiveEntity
#define __PrimitiveEntity

#include "Entity.hpp"
#include "GFX/Scenery/Scene.hpp"
#include "GFX/Scenery/Renderer.hpp"
#include "GFX/Scenery/RCDrawComponent.hpp"
#include "GTransformComponent.hpp"
#include "GFX/Scenery/TransformDrawContexts.hpp"
#include "InputComponent.hpp"
#include "InputSystem.hpp"
#include "GFX/Scenery/Camera.hpp"

#include "App/ChiliWindow.hpp"
#include "GFX/Core/Factory.hpp"
#include "GFX/Core/Storage.hpp"
#include "GFX/Core/Pipeline.hpp"
#include "GFX/PipelineObjects/IA.hpp"

#include "GFX/PipelineObjects/Topology.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/PipelineObjects/Shader.hpp"
#include "GFX/PipelineObjects/Viewport.hpp"

#include <d3d11.h>
#include "GFX/Core/Namespaces.hpp"
#include "GFX/Core/Exception.hpp"

#include <vector>
#include <memory>

struct PEFaceColorData {
    gfx::GFXColor faceColors[6];
};

class PETopology : public gfx::po::Topology {
public:
    PETopology()
        : gfx::po::Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}
};

class PETransformCBuf : public gfx::po::VSCBuffer<dx::XMMATRIX>{
public:
    PETransformCBuf() = default;
    PETransformCBuf(gfx::GFXFactory factory)
        : gfx::po::VSCBuffer<dx::XMMATRIX>(factory, D3D11_USAGE_DYNAMIC,
            D3D11_CPU_ACCESS_WRITE, initialTransforms()
        ) {}

private:
    static std::vector<dx::XMMATRIX> initialTransforms();
};

class PEIndexedColorCBuf : public gfx::po::PSCBuffer<PEFaceColorData> {
public:
    PEIndexedColorCBuf() = default;
    PEIndexedColorCBuf(gfx::GFXFactory factory)
        : gfx::po::PSCBuffer<PEFaceColorData>(
            factory, D3D11_USAGE_DEFAULT, 0u, initialColors()
        ) {}

private:
    static std::vector<PEFaceColorData> initialColors();
};

class PEColorBuffer : public gfx::po::VertexBuffer<gfx::GFXColor> {
public:
    PEColorBuffer(gfx::GFXFactory factory, std::size_t size)
        : gfx::po::VertexBuffer<gfx::GFXColor>( factory, makeRandom(size) ) {}

private:
    static std::vector<gfx::GFXColor> makeRandom(std::size_t size);
};

class PEViewport : public gfx::po::Viewport {
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

template < class T, class PosBufferT, class IndexBufferT >
class PEDrawComponent : public gfx::scenery::RCDrawCmp {
public:
    struct {} tagPosBuffer;
    struct {} tagIndexBuffer;
    struct {} tagTopology;
    struct {} tagTransformCBuf;
    struct {} tagIndexedColorCBuf;
    struct {} tagBlendedColorBuffer;
    struct {} tagViewport;

    using MyType = gfx::scenery::DrawComponent<T>;
    using MyVertex = gfx::GFXVertex;
    using MyIndex = gfx::GFXIndex;
    using MyColor = gfx::GFXColor;
    using MyConstantBufferColor = PEFaceColorData;
    using MyPosBuffer = PosBufferT;
    using MyIndexBuffer = IndexBufferT;
    using MyTopology = PETopology;
    using MyTransformCBuf = PETransformCBuf;
    using MyIndexedColorCBuf = PEIndexedColorCBuf;
    using MyBlendedColorBuffer = PEColorBuffer;
    using MyViewport = PEViewport;
    using MyDrawCaller = gfx::po::DrawCallerIndexed;

    PEDrawComponent( gfx::GFXFactory factory, gfx::GFXPipeline pipeline,
        gfx::GFXStorage& storage, const ChiliWindow& wnd
    ) : transformGPUMapper_(storage),
        transformApplyer_(transformGPUMapper_),
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        posBuffer_( gfx::GFXRes::makeLoaded<MyPosBuffer>(storage, factory) ),
        indexBuffer_( gfx::GFXRes::makeLoaded<MyIndexBuffer>(storage, factory) ),
        topology_( gfx::GFXRes::makeCached<MyTopology>(storage, tagTopology) ),
        viewport_( gfx::GFXRes::makeCached<MyViewport>(storage, tagViewport, wnd.client()) ),
        transformCBuf_( gfx::GFXRes::makeCached<MyTransformCBuf>(storage, tagTransformCBuf, factory) ),
        indexedColorCBuf_( gfx::GFXRes::makeCached<MyIndexedColorCBuf>(storage, tagIndexedColorCBuf, factory) ),
        blendedColorBuffer_( gfx::GFXRes::makeLoaded<MyBlendedColorBuffer>(
            storage, factory, MyPosBuffer::size()
        ) ),
        pipeline_(pipeline), pStorage_(&storage) {

        transformGPUMapper_.setTCBufID(transformCBuf_.id());

        this->setDrawCaller( std::make_unique<MyDrawCaller>(
            static_cast<UINT>( MyIndexBuffer::size() ), 0u, 0
        ) );

        this->drawCaller().addDrawContext(&transformApplyer_);
        this->drawCaller().addDrawContext(&transformGPUMapper_);
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_.entryStackPop();
    #endif

    }

    template <class ... TesselationFactors>
    PEDrawComponent( gfx::GFXFactory factory, gfx::GFXPipeline pipeline,
        gfx::GFXStorage& storage, const ChiliWindow& wnd,
        TesselationFactors&& ... tesselationFactors
    ) : transformGPUMapper_(storage),
        transformApplyer_(transformGPUMapper_),
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        posBuffer_( gfx::GFXRes::makeLoaded<MyPosBuffer>( storage, factory,
            std::forward<TesselationFactors>(tesselationFactors)...
        ) ),
        indexBuffer_( gfx::GFXRes::makeLoaded<MyIndexBuffer>( storage, factory,
            std::forward<TesselationFactors>(tesselationFactors)...
        ) ),
        topology_( gfx::GFXRes::makeCached( storage, tagTopology ) ),
        viewport_( gfx::GFXRes::makeCached( storage, tagViewport, wnd.client() ) ),
        transformCBuf_( gfx::GFXRes::makeCached( storage, tagTransformCBuf, factory ) ),
        indexedColorCBuf_( gfx::GFXRes::makeCached( storage, tagIndexedColorCBuf, factory ) ),
        blendedColorBuffer_( gfx::GFXRes::makeLoaded(storage, factory, MyPosBuffer::size(
            std::forward<TesselationFactors>(tesselationFactors)...
        ) ) ),
        pipeline_(pipeline), pStorage_(&storage) {

        transformGPUMapper_.setTCBufID(transformCBuf_.id());

        this->setDrawCaller( std::make_unique<MyDrawCaller>(
            static_cast<UINT>( MyIndexBuffer::size(
                std::forward<TesselationFactors>(tesselationFactors)...
            ) ), 0u, 0
        ) );

        this->drawCaller().addDrawContext(&transformApplyer_);
        this->drawCaller().addDrawContext(&transformGPUMapper_);
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_.entryStackPop();
    #endif

    }

    void update(const gfx::Transform transform) {
        transformGPUMapper_.update(transform);
    }

    void sync(const gfx::scenery::Renderer& renderer) override {
        if ( typeid(renderer) == typeid(gfx::scenery::IndexedRenderer) ) {
            sync( static_cast<const gfx::scenery::IndexedRenderer&>(renderer) );
        }
        else if ( typeid(renderer) == typeid(gfx::scenery::BlendedRenderer) ) {
            sync( static_cast<const gfx::scenery::BlendedRenderer&>(renderer) );
        }
        else {
            throw GFX_EXCEPT_CUSTOM("DrawComponent tried to synchronize with incompatible renderer.\n");
        }
    }
    
    void sync(const gfx::scenery::IndexedRenderer& renderer) {
        assert(posBuffer_.valid());
        static_cast<MyPosBuffer*>( posBuffer_.get() )
            ->setSlot( gfx::scenery::IndexedRenderer::slotPosBuffer() );

        assert(transformCBuf_.valid());
        static_cast<MyTransformCBuf*>( transformCBuf_.get() )
            ->setSlot( 0u );

        this->setRODesc( gfx::scenery::RenderObjectDesc{
            .header = {
                .IDBuffer = posBuffer_.id(),
                .IDType = typeid(T)
            },
            .IDs = {
                posBuffer_.id(), indexBuffer_.id(), topology_.id(),
                viewport_.id(), transformCBuf_.id(), indexedColorCBuf_.id()
            }
        } );
    }

    void sync(const gfx::scenery::BlendedRenderer& renderer) {
        assert(posBuffer_.valid());
        static_cast<MyPosBuffer*>( posBuffer_.get() )
            ->setSlot( gfx::scenery::BlendedRenderer::slotPosBuffer() );
            
        assert(blendedColorBuffer_.valid());
        static_cast<MyBlendedColorBuffer*>(
            blendedColorBuffer_.get()
        )->setSlot( gfx::scenery::BlendedRenderer::slotColorBuffer() );

        assert(transformCBuf_.valid());
        static_cast<MyTransformCBuf*>( transformCBuf_.get() )
            ->setSlot( 0u );

        this->setRODesc( gfx::scenery::RenderObjectDesc{
            .header = {
                .IDBuffer = posBuffer_.id(),
                .IDType = typeid(T)
            },
            .IDs = {
                posBuffer_.id(), blendedColorBuffer_.id(), indexBuffer_.id(), topology_.id(),
                viewport_.id(), transformCBuf_.id()
            }
        } );
    }

    void sync(const gfx::scenery::CameraVision& vision) {
        transformApplyer_.setTransform(
            vision.viewTrans() * vision.projTrans()
        );
    }

private:
    gfx::scenery::MapTransformGPU transformGPUMapper_;
    gfx::scenery::ApplyTransform transformApplyer_;
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
    gfx::scenery::RCDrawCmp::LogComponent logComponent_;
#endif
    gfx::GFXRes posBuffer_;
    gfx::GFXRes indexBuffer_;
    gfx::GFXRes topology_;
    gfx::GFXRes viewport_;
    gfx::GFXRes transformCBuf_;
    gfx::GFXRes indexedColorCBuf_;
    gfx::GFXRes blendedColorBuffer_;
    gfx::GFXPipeline pipeline_;
    gfx::GFXStorage* pStorage_;
};

template <class T>
class PrimitiveEntity : public IEntity {
public:
    friend class Loader<PrimitiveEntity>;

    PrimitiveEntity()
        : dc_(), tc_(), ic_() {}

    void update(milliseconds elapsed) override {
        tc_->update(elapsed);
        tc_->setTotal( tc_->local() * tc_->global() );
        dc_->updateTrans(tc_->total());
    }

    // ct stands for construct
    template <class ... TesselationFactors>
    void ctDrawComponent(gfx::GFXFactory factory, gfx::GFXPipeline pipeline,
        gfx::GFXStorage& storage, const ChiliWindow& wnd,
        TesselationFactors&& ... tesselationFactors
    ) {
        dc_.reset( new gfx::scenery::DrawComponent<T>(factory, pipeline, storage, wnd,
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
    std::shared_ptr< gfx::scenery::DrawComponent<T> >
    drawComponent() const noexcept {
        return dc_;
    }

    std::shared_ptr<gfx::BasicTransformComponent>
    transformComponent() const noexcept {
        return tc_;
    }

    std::shared_ptr< gfx::scenery::DrawComponent<T> > dc_;
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

    void loadAt(gfx::scenery::Scene& scene, std::size_t idxLayer = 0u) {
        scene.layer(idxLayer).addDrawCmp(entity_.drawComponent().get());
    }

    void loadAt(gfx::scenery::LSceneAdapter scene) {
        scene.addDrawCmp(entity_.drawComponent().get());
    }

private:
    const PrimitiveEntity<T>& entity_;
};

template <class T>
Loader<PrimitiveEntity<T>> PrimitiveEntity<T>::loader() const noexcept {
    return Loader<PrimitiveEntity<T>>(*this);
}

#endif  // __PrimitiveEntity