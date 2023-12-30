#ifndef __Sheet
#define __Sheet

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "DrawComponent.hpp"

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
class DrawComponent<Sheet> : public IDrawComponent {
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
    ) :
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        RODesc_(),
        drawCaller_( static_cast<UINT>( MyIndexBuffer::size() ), 0u, 0 ),
        pipeline_(pipeline), pStorage_(&storage),
        IDPosBuffer_( storage.cache<MyVertexBuffer>(factory) ),
        IDTexBuffer_( storage.cache<MyTexBuffer>(factory) ),
        IDIndexBuffer_( storage.cache<MyIndexBuffer>(factory) ),
        IDTopology_( storage.cache<MyTopology>() ),
        IDViewport_( storage.cache<MyViewport>( wnd.client() ) ),
        IDTransformCBuf_( storage.cache<MyTransformCBuf>(factory) ),
        IDTexture_( storage.cache<MyTexture>(factory) ),
        IDSampler_( storage.cache<MySampler>(factory) ),
        transformGPUMapper_( std::make_shared<MapTransformGPU>(
            storage, IDTransformCBuf_
        ) ),
        transformApplyer_( std::make_shared<ApplyTransform>(
            *transformGPUMapper_
        ) ) {
        drawCaller_.addDrawContext(transformApplyer_);
        drawCaller_.addDrawContext(transformGPUMapper_);
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_.entryStackPop();
    #endif
    }

    void update(const Transform transform) {
        transformGPUMapper_->update(transform);
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
                .IDType = typeid(Sheet)
            },
            .IDs = {
                IDPosBuffer_, IDTexBuffer_, IDIndexBuffer_,
                IDTopology_, IDViewport_, IDTransformCBuf_,
                IDTexture_, IDSampler_
            }
        };
    }

    void sync(const CameraVision& vision) {
        transformApplyer_->setTransform(
            vision.viewTransComp().total()
            * vision.projTransComp().total()
        );
    }

    const BasicDrawCaller* drawCaller() const override {
        return &drawCaller_;
    }

    BasicDrawCaller* drawCaller() override {
        return &drawCaller_;
    }

private:
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
    IDrawComponent::LogComponent logComponent_;
#endif
    std::optional<RenderObjectDesc> RODesc_;
    MyDrawCaller drawCaller_;
    GFXPipeline pipeline_;
    GFXStorage* pStorage_;
    GFXStorage::ID IDPosBuffer_;
    GFXStorage::ID IDTexBuffer_;
    GFXStorage::ID IDIndexBuffer_;
    GFXStorage::ID IDTopology_;
    GFXStorage::ID IDViewport_;
    GFXStorage::ID IDTransformCBuf_;
    GFXStorage::ID IDTexture_;
    GFXStorage::ID IDSampler_;
    // GPU Mapper must be declared at here
    // as it requires transform cbuffer already constructed.
    std::shared_ptr<MapTransformGPU> transformGPUMapper_;
    // transform applyer must be declared at here
    // as it requires GPU Mapper reference.
    std::shared_ptr<ApplyTransform> transformApplyer_;
};

template<>
class Entity<Sheet> : public PrimitiveEntity<Sheet> {

};

#endif  // __Sheet