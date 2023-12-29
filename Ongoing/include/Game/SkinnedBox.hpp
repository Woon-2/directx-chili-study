#ifndef __SkinnedBox
#define __SkinnedBox

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "DrawComponent.hpp"

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
class DrawComponent<SkinnedBox> : public IDrawComponent {
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
    class MyDrawContext : public DrawContextBasic {
    public:
        MyDrawContext( GFXStorage& mappedStorage,
            GFXStorage::ID IDTransCBuf
        ) : DrawContextBasic(36u, 0u), trans_(),
            mappedStorage_(&mappedStorage), IDTransCBuf_(IDTransCBuf) {}

    void update(const Transform trans) {
        trans_ = trans;
    }

    private:
        void drawCall(GFXPipeline& pipeline) const override {
            // first adopt transform
            assert( mappedStorage_->get(IDTransCBuf_).has_value() );

            auto transCBuf = static_cast<PETransformCBuf*>(
                mappedStorage_->get(IDTransCBuf_).value()
            );

            transCBuf->dynamicUpdate(pipeline, [this](){ return trans_.data(); });
            // then draw
            DrawContextBasic::basicDrawCall(pipeline);
        }

        Transform trans_;
        GFXStorage* mappedStorage_;
        GFXStorage::ID IDTransCBuf_;
    };

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
    ) :
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        RODesc_(), pipeline_(pipeline), pStorage_(&storage),
        IDPosBuffer_( storage.cache<MyVertexBuffer>(factory) ),
        IDTexBuffer_( storage.cache<MyTexBuffer>(factory) ),
        IDTopology_( storage.cache<MyTopology>() ),
        IDViewport_( storage.cache<MyViewport>( wnd.client() ) ),
        IDTransformCBuf_( storage.cache<MyTransformCBuf>(factory) ),
        IDTexture_( storage.cache<MyTexture>(factory) ),
        IDSampler_( storage.cache<MySampler>(factory) ),
        drawContext_( storage, IDTransformCBuf_ ) {
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
        if ( typeid(renderer) == typeid(TexturedRenderer) ) {
            sync( static_cast<const TexturedRenderer&>(renderer) );
        }
        else {
            throw GFX_EXCEPT_CUSTOM("DrawComponent tried to synchronize with incompatible renderer.\n");
        }
    }

    void sync(const TexturedRenderer& renderer) {
        assert(pStorage_->get(IDTransformCBuf_).has_value());
        static_cast<MyTransformCBuf*>( pStorage_->get(IDTransformCBuf_).value() )
            ->setSlot( 0u );

        assert(pStorage_->get(IDPosBuffer_).has_value());
        static_cast<MyVertexBuffer*>( pStorage_->get(IDPosBuffer_).value() )
            ->setSlot( TexturedRenderer::slotPosBuffer() );

        assert(pStorage_->get(IDTexBuffer_).has_value());
        static_cast<MyTexBuffer*>( pStorage_->get(IDTexBuffer_).value() )
            ->setSlot( TexturedRenderer::slotTexBuffer() );

        assert(pStorage_->get(IDTexture_).has_value());
        static_cast<MyTexture*>( pStorage_->get(IDTexture_).value() )
            ->setSlot( TexturedRenderer::slotTexture() );

        assert(pStorage_->get(IDSampler_).has_value());
        static_cast<MySampler*>( pStorage_->get(IDSampler_).value() )
            ->setSlot( TexturedRenderer::slotSamplerState() );

        RODesc_ = RenderObjectDesc{
            .header = {
                .IDBuffer = IDPosBuffer_,
                .IDType = typeid(SkinnedBox)
            },
            .IDs = {
                IDPosBuffer_, IDTexBuffer_,
                IDTopology_, IDViewport_, IDTransformCBuf_,
                IDTexture_, IDSampler_
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
    GFXStorage::ID IDTexBuffer_;
    GFXStorage::ID IDTopology_;
    GFXStorage::ID IDViewport_;
    GFXStorage::ID IDTransformCBuf_;
    GFXStorage::ID IDTexture_;
    GFXStorage::ID IDSampler_;
    // draw context is here to guarantee
    // it is initalized at the last.
    // since draw context may use other member data,
    // to protect the program from accessing unitialized data,
    // sacrifice memory efficiency.
    MyDrawContext drawContext_;
};

template<>
class Entity<SkinnedBox> : public PrimitiveEntity<SkinnedBox> {

};

#endif  // __SkinnedBox