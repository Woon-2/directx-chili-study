#ifndef __SkinnedBox
#define __SkinnedBox

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "RCDrawComponent.hpp"

#include "GFX/Primitives/Cube.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"

#include "Image/Surface.hpp"
#include "GFX/PipelineObjects/Texture.hpp"
#include "GFX/PipelineObjects/Sampler.hpp"

#include <vector>
#include <array>
#include <iterator>

#include "Resource.hpp"

class SkinnedBox {

};

template <>
class DrawComponent<SkinnedBox> : public RCDrawCmp {
public:
    class MyVertexBuffer : public Primitives::Cube::CubeVertexBufferIndependent {
    public:
        MyVertexBuffer(GFXFactory factory)
            : Primitives::Cube::CubeVertexBufferIndependent(factory) {}

        static constexpr auto size() {
            return Primitives::Cube::CubeVertexBufferIndependent::size();
        }
    };

    class MyTexBuffer : public VertexBuffer<GFXTexCoord> {
    public:
        MyTexBuffer(GFXFactory factory)
            : VertexBuffer<GFXTexCoord>( factory, initialTexCoords() ) {}

    private:
        static std::vector<GFXTexCoord> initialTexCoords() {
            static constexpr auto tc = std::array<GFXTexCoord, 14>{
                GFXTexCoord{2.f/3.f, 0.f/4.f}, // 0
                GFXTexCoord{1.f/3.f, 0.f/4.f}, // 1
                GFXTexCoord{2.f/3.f, 1.f/4.f}, // 2 
                GFXTexCoord{1.f/3.f, 1.f/4.f}, // 3
                GFXTexCoord{2.f/3.f, 3.f/4.f}, // 4
                GFXTexCoord{1.f/3.f, 3.f/4.f}, // 5 
                GFXTexCoord{2.f/3.f, 2.f/4.f}, // 6
                GFXTexCoord{1.f/3.f, 2.f/4.f}, // 7
                GFXTexCoord{2.f/3.f, 4.f/4.f}, // 8
                GFXTexCoord{1.f/3.f, 4.f/4.f}, // 9
                GFXTexCoord{3.f/3.f, 1.f/4.f}, // 10
                GFXTexCoord{3.f/3.f, 2.f/4.f}, // 11
                GFXTexCoord{0.f/3.f, 1.f/4.f}, // 12 
                GFXTexCoord{0.f/3.f, 2.f/4.f}  // 13
            };

            auto ret = std::vector<GFXTexCoord>();
            ret.reserve(36u);

            auto out = std::back_inserter(ret);
            out = tc[0]; out = tc[2]; out = tc[1]; out = tc[2]; out = tc[3]; out = tc[1];
            out = tc[12]; out = tc[3]; out = tc[13]; out = tc[3]; out = tc[7]; out = tc[13];
            out = tc[2]; out = tc[6]; out = tc[3]; out = tc[3]; out = tc[6]; out = tc[7];
            out = tc[4]; out = tc[5]; out = tc[7]; out = tc[4]; out = tc[7]; out = tc[6];
            out = tc[10]; out = tc[11]; out = tc[2]; out = tc[2]; out = tc[11]; out = tc[6];
            out = tc[8]; out = tc[9]; out = tc[4]; out = tc[9]; out = tc[5]; out = tc[4];

            return ret;
        }
    };

    using MyTopology = PETopology;
    using MyTransformCBuf = PETransformCBuf;
    using MyViewport = PEViewport;
    using MyDrawCaller = DrawCaller;

    class MyTexture : public Texture {
    public:
        MyTexture(GFXFactory factory)
            : Texture(factory, initialSurface()) {}

    private:
        static Surface initialSurface() {
            return Surface::FromFile( (resourcePath/L"cube.png").native() );
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
        topology_( GFXMappedResource::Type<MyTopology>{},
            typeid(MyTopology), storage
        ),
        viewport_( GFXMappedResource::Type<MyViewport>{},
            typeid(MyViewport), storage,  wnd.client()
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
            static_cast<UINT>( MyVertexBuffer::size() ), 0
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
                .IDType = typeid(SkinnedBox)
            },
            .IDs = {
                posBuffer_.id(), texBuffer_.id(),
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
    GFXMappedResource topology_;
    GFXMappedResource viewport_;
    GFXMappedResource transformCBuf_;
    GFXMappedResource texture_;
    GFXMappedResource sampler_;
    std::optional<RenderObjectDesc> RODesc_;
    MyDrawCaller drawCaller_;
    GFXPipeline pipeline_;
    GFXStorage* pStorage_;
};

template<>
class Entity<SkinnedBox> : public PrimitiveEntity<SkinnedBox> {

};

#endif  // __SkinnedBox