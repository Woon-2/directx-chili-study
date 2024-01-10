#ifndef __Sheet
#define __Sheet

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "RCDrawComponent.hpp"

#include "GFX/Primitives/Plane.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"

#include "Image/Surface.hpp"
#include "GFX/PipelineObjects/Texture.hpp"
#include "GFX/PipelineObjects/Sampler.hpp"

#include "Resource.hpp"

class Sheet {

};

template <>
class DrawComponent<Sheet> : public RCDrawCmp {
public:
    class MyVertexBuffer : public Primitives::Plane::PlaneVertexBuffer {
    public:
        MyVertexBuffer(GFXFactory factory)
            : Primitives::Plane::PlaneVertexBuffer(factory, 1u, 1u) {}
    };

    class MyTexBuffer : public VertexBuffer<GFXTexCoord> {
    public:
        MyTexBuffer(GFXFactory factory)
            : VertexBuffer<GFXTexCoord>( factory, initialTexCoords() ) {}

    private:
        static std::vector<GFXTexCoord> initialTexCoords() {
            return std::vector<GFXTexCoord>{
                GFXTexCoord{0.f, 0.f},  // bottom-left
                GFXTexCoord{1.f, 0.f},  // bottom-right
                GFXTexCoord{0.f, 1.f},  // top-left
                GFXTexCoord{1.f, 1.f},  // top-right
            };
        }
    };

    class MyIndexBuffer : public Primitives::Plane::PlaneIndexBuffer {
    public:
        MyIndexBuffer(GFXFactory factory)
            : Primitives::Plane::PlaneIndexBuffer(factory, 1u, 1u) {}

        static constexpr std::size_t size() noexcept {
            return 6u;
        }
    };

    using MyTopology = PETopology;
    using MyTransformCBuf = PETransformCBuf;
    using MyViewport = PEViewport;
    using MyDrawCaller = DrawCallerIndexed;

    class MyTexture : public Texture {
    public:
        MyTexture(GFXFactory factory)
            : Texture(factory, initialSurface()) {}

    private:
        static Surface initialSurface() {
            return Surface::FromFile( (resourcePath/L"kappa50.png").native() );
        }
    };

    class MySampler : public Sampler {
    public:
        MySampler(GFXFactory factory)
            : Sampler(factory) {}
    };

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        GFXStorage& storage, const ChiliWindow& wnd
    ) : transformGPUMapper_(storage),
        transformApplyer_(transformGPUMapper_),
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        posBuffer_( GFXMappedResource::Type<MyVertexBuffer>{},
            typeid(MyVertexBuffer), storage, factory
        ),
        texBuffer_( GFXMappedResource::Type<MyTexBuffer>{},
            typeid(MyTexBuffer), storage, factory
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
        texture_( GFXMappedResource::Type<MyTexture>{},
            typeid(MyTexture), storage, factory
        ),
        sampler_( GFXMappedResource::Type<MySampler>{},
            typeid(MySampler), storage, factory
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

    void update(const Transform transform) {
        transformGPUMapper_.update(transform);
    }

    void sync(const Renderer& renderer) override {
        if ( typeid(renderer) == typeid(TexturedRenderer) ) {
            sync( static_cast<const TexturedRenderer&>(renderer) );
        }
        else {
            throw GFX_EXCEPT_CUSTOM("DrawComponent tried to synchronize with incompatible renderer.\n");
        }
    }

    void sync(const TexturedRenderer& renderer) {
        assert(transformCBuf_.valid());
        static_cast<MyTransformCBuf*>( transformCBuf_.get() )
            ->setSlot( 0u );

        assert(posBuffer_.valid());
        static_cast<MyVertexBuffer*>( posBuffer_.get() )
            ->setSlot( TexturedRenderer::slotPosBuffer() );

        assert(texBuffer_.valid());
        static_cast<MyTexBuffer*>( texBuffer_.get() )
            ->setSlot( TexturedRenderer::slotTexBuffer() );

        assert(texture_.valid());
        static_cast<MyTexture*>( texture_.get() )
            ->setSlot( TexturedRenderer::slotTexture() );

        assert(sampler_.valid());
        static_cast<MySampler*>( sampler_.get() )
            ->setSlot( TexturedRenderer::slotSamplerState() );

        this->setRODesc( RenderObjectDesc{
            .header = {
                .IDBuffer = posBuffer_.id(),
                .IDType = typeid(Sheet)
            },
            .IDs = {
                posBuffer_.id(), texBuffer_.id(), indexBuffer_.id(),
                topology_.id(), viewport_.id(), transformCBuf_.id(),
                texture_.id(), sampler_.id()
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
    IDrawComponent::LogComponent logComponent_;
#endif
    GFXMappedResource posBuffer_;
    GFXMappedResource texBuffer_;
    GFXMappedResource indexBuffer_;
    GFXMappedResource topology_;
    GFXMappedResource viewport_;
    GFXMappedResource transformCBuf_;
    GFXMappedResource texture_;
    GFXMappedResource sampler_;
    GFXPipeline pipeline_;
    GFXStorage* pStorage_;
};

template<>
class Entity<Sheet> : public PrimitiveEntity<Sheet> {

};

#endif  // __Sheet