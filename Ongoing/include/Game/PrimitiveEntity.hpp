#ifndef __PrimitiveEntity
#define __PrimitiveEntity

#include "Entity.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "RCDrawComponent.hpp"
#include "GTransformComponent.hpp"
#include "TransformDrawContexts.hpp"
#include "InputComponent.hpp"
#include "InputSystem.hpp"
#include "Camera.hpp"

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
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

#include <vector>
#include <memory>

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
        : VSCBuffer<dx::XMMATRIX>(factory, D3D11_USAGE_DYNAMIC,
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
            factory, D3D11_USAGE_DEFAULT, 0u, initialColors()
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

template < class T, class PosBufferT, class IndexBufferT >
class PEDrawComponent : public RCDrawCmp {
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
    using MyDrawCaller = DrawCallerIndexed;

    PEDrawComponent( GFXFactory factory, GFXPipeline pipeline,
        GFXStorage& storage, const ChiliWindow& wnd
    ) : transformGPUMapper_(storage),
        transformApplyer_(transformGPUMapper_),
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        posBuffer_( GFXMappedResource::Type<MyPosBuffer>{},
            typeid(MyPosBuffer), storage, factory
        ),
        indexBuffer_( GFXMappedResource::Type<MyIndexBuffer>{},
            typeid(MyIndexBuffer), storage, factory
        ),
        topology_( GFXMappedResource::Type<MyTopology>{},
            typeid(MyTopology), storage
        ),
        viewport_( GFXMappedResource::Type<MyViewport>{},
            typeid(MyViewport), storage, wnd.client()
        ),
        transformCBuf_( GFXMappedResource::Type<MyTransformCBuf>{},
            typeid(MyTransformCBuf), storage, factory
        ),
        indexedColorCBuf_( GFXMappedResource::Type<MyIndexedColorCBuf>{},
            typeid(MyIndexedColorCBuf), storage, factory
        ),
        blendedColorBuffer_( GFXMappedResource::Type<MyBlendedColorBuffer>{},
            typeid(MyBlendedColorBuffer), storage,
            factory, MyPosBuffer::size()
        ),
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
    PEDrawComponent( GFXFactory factory, GFXPipeline pipeline,
        GFXStorage& storage, const ChiliWindow& wnd,
        TesselationFactors&& ... tesselationFactors
    ) : transformGPUMapper_(storage),
        transformApplyer_(transformGPUMapper_),
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        posBuffer_( GFXMappedResource::Type<MyPosBuffer>{}, storage, factory,
            std::forward<TesselationFactors>(tesselationFactors)...
        ),
        indexBuffer_( GFXMappedResource::Type<MyIndexBuffer>{}, storage, factory,
            std::forward<TesselationFactors>(tesselationFactors)...
        ),
        topology_( GFXMappedResource::Type<MyTopology>{},
            typeid(MyTopology), storage
        ),
        viewport_( GFXMappedResource::Type<MyViewport>{},
            typeid(MyViewport), storage, wnd.client()
        ),
        transformCBuf_( GFXMappedResource::Type<MyTransformCBuf>{},
            typeid(MyTransformCBuf), storage, factory 
        ),
        indexedColorCBuf_( GFXMappedResource::Type<MyIndexedColorCBuf>{},
            typeid(MyIndexedColorCBuf), storage, factory
        ),
        blendedColorBuffer_( GFXMappedResource::Type<MyBlendedColorBuffer>{},
            typeid(MyBlendedColorBuffer), storage,
            factory, MyPosBuffer::size(
                std::forward<TesselationFactors>(tesselationFactors)...
            )
        ),
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

    void update(const Transform transform) {
        transformGPUMapper_.update(transform);
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
        assert(posBuffer_.valid());
        static_cast<MyPosBuffer*>( posBuffer_.get() )
            ->setSlot( IndexedRenderer::slotPosBuffer() );

        assert(transformCBuf_.valid());
        static_cast<MyTransformCBuf*>( transformCBuf_.get() )
            ->setSlot( 0u );

        this->setRODesc( RenderObjectDesc{
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

    void sync(const BlendedRenderer& renderer) {
        assert(posBuffer_.valid());
        static_cast<MyPosBuffer*>( posBuffer_.get() )
            ->setSlot( BlendedRenderer::slotPosBuffer() );
            
        assert(blendedColorBuffer_.valid());
        static_cast<MyBlendedColorBuffer*>(
            blendedColorBuffer_.get()
        )->setSlot( BlendedRenderer::slotColorBuffer() );

        assert(transformCBuf_.valid());
        static_cast<MyTransformCBuf*>( transformCBuf_.get() )
            ->setSlot( 0u );

        this->setRODesc( RenderObjectDesc{
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

    void sync(const CameraVision& vision) {
        transformApplyer_.setTransform(
            vision.viewTrans() * vision.projTrans()
        );
    }

private:
    MapTransformGPU transformGPUMapper_;
    ApplyTransform transformApplyer_;
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
    RCDrawCmp::LogComponent logComponent_;
#endif
    GFXMappedResource posBuffer_;
    GFXMappedResource indexBuffer_;
    GFXMappedResource topology_;
    GFXMappedResource viewport_;
    GFXMappedResource transformCBuf_;
    GFXMappedResource indexedColorCBuf_;
    GFXMappedResource blendedColorBuffer_;
    GFXPipeline pipeline_;
    GFXStorage* pStorage_;
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

    void loadAt(Scene& scene, std::size_t idxLayer = 0u) {
        scene.layer(idxLayer).addDrawCmp(entity_.drawComponent().get());
    }

    void loadAt(LSceneAdapter scene) {
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