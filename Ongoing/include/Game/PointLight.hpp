#ifndef __PointLight
#define __PointLight

#include "GFX/PipelineObjects/Bindable.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/GraphicsStorage.hpp"

#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

#include "CoordSystem.hpp"
#include "Camera.hpp"

#include <ranges>
#include <optional>

struct BPPointLightDesc {
    dx::XMFLOAT3A pos;
    dx::XMFLOAT3 color;
    float attConst;
    float attLin;
    float attQuad;
};

// for forward declaration of Utilized::BPDynPointLight,
// it is required for specifying Utilized::BPDynPointLight as friend
// in Basic::BPDynPointLight.
namespace Utilized {
    class BPDynPointLight;
}

namespace Basic {

class BPDynPointLight : public IBindable{
private:
    using MyPSCBuffer = PSCBuffer<BPPointLightDesc>;

public:
    friend class Utilized::BPDynPointLight;

    BPDynPointLight()
        : BPDynPointLight( defLightDesc() ) {}

    BPDynPointLight(const BPPointLightDesc& lightDesc)
        : lightDesc_(lightDesc), res_(), dirty_(false) {}

    BPDynPointLight(GFXFactory factory, GFXStorage& storage)
        : BPDynPointLight( std::move(factory), storage, defLightDesc() ) {}

    BPDynPointLight(GFXFactory factory)
        : BPDynPointLight( std::move(factory), defLightDesc() ) {}

    BPDynPointLight(GFXFactory factory, const BPPointLightDesc& lightDesc)
        : lightDesc_(lightDesc), res_(), dirty_(false) {
        config(std::move(factory));
    }

    BPDynPointLight( GFXFactory factory, GFXStorage& storage,
        const BPPointLightDesc& lightDesc
    ) : lightDesc_(lightDesc),
        res_( GFXMappedResource::Type<MyPSCBuffer>{}, storage,
            // the resource construction arguments follow.
            std::move(factory), D3D11_USAGE_DEFAULT, 0,
            std::ranges::single_view(lightDesc_)
        ), dirty_(false) {}

    void sync(GFXStorage& storage) {
        res_.sync(storage);
        res_.remap();
    }

    void config(GFXFactory factory) {
        res_.config<MyPSCBuffer>( std::move(factory), D3D11_USAGE_DEFAULT, 0,
            std::ranges::single_view(lightDesc_)
        );
    }

    const BPPointLightDesc& lightDesc() const noexcept {
        return lightDesc_;
    }

    const dx::XMVECTOR VCALL pos() const noexcept {
        return dx::XMLoadFloat3(&lightDesc_.pos);
    }

    const dx::XMVECTOR VCALL color() const noexcept {
        return dx::XMLoadFloat3(&lightDesc_.color);
    }

    const float attConst() const noexcept {
        return lightDesc_.attConst;
    }

    const float attLin() const noexcept {
        return lightDesc_.attLin;
    }

    const float attQuad() const noexcept {
        return lightDesc_.attQuad;
    }

    void VCALL setPos(dx::FXMVECTOR posVal) {
        dx::XMStoreFloat3A(&lightDesc_.pos, posVal);
        dirty_ = true;
    }

    void VCALL setColor(dx::FXMVECTOR colorVal) {
        dx::XMStoreFloat3(&lightDesc_.color, colorVal);
        dirty_ = true;
    }

    void setAttConst(float attConstVal) {
        lightDesc_.attConst = attConstVal;
        dirty_ = true;
    }

    void setAttLin(float attLinVal) {
        lightDesc_.attLin = attLinVal;
        dirty_ = true;
    }

    void setAttQuad(float attQuadVal) {
        lightDesc_.attQuad = attQuadVal;
        dirty_ = true;
    }

    UINT slot() const {
        return res_.as<MyPSCBuffer>().slot();
    }

    void setSlot(UINT val) {
        auto& res = res_.as<MyPSCBuffer>();
        res.setSlot(val);
    }

private:
    // referenced attenuation coefficient constants from
    // https://wiki.ogre3d.org/-Point+Light+Attenuation
    static const BPPointLightDesc defLightDesc() noexcept {
        return BPPointLightDesc{
            .pos = dx::XMFLOAT3A(0.f, 0.f, 0.f),
            .color = dx::XMFLOAT3(1.f, 1.f, 1.f),
            .attConst = 1.f,
            .attLin = 0.14f,
            .attQuad = 0.07f
        };
    }

    void bind(GFXPipeline& pipeline) override {
        // if rebind required, rebind
        // it is handled in PSCBuffer internally
        pipeline.bind(res_.get());

        if (dirty_) {
            res_.as<MyPSCBuffer>().stage( pipeline, [this](){
                return &lightDesc_;
            } );
            dirty_ = false;
        }
    }

    BPPointLightDesc lightDesc_;
    // res_ depends on lightDesc_ in initialization.
    // so res_ should be here.
    GFXMappedResource res_;
    bool dirty_;
};

} // namespace Basic

namespace Utilized {

// utilize CoordSystem and CameraVision.
// bind its position as on view space.
// if more modification is needed, adopt decorater pattern.
class BPDynPointLight : public IBindable {
public:
    BPDynPointLight()
        : coord_(), base_(), pivot_(coord_) {}

    BPDynPointLight(const BPPointLightDesc& lightDesc)
        : coord_(), base_(lightDesc), pivot_(coord_) {}

    BPDynPointLight(GFXFactory factory, GFXStorage& storage)
        : coord_(), base_(std::move(factory), storage), pivot_(coord_) {}

    BPDynPointLight(GFXFactory factory)
        : coord_(), base_(std::move(factory)), pivot_(coord_) {}

    BPDynPointLight(GFXFactory factory, const BPPointLightDesc& lightDesc)
        : coord_(), base_(std::move(factory), lightDesc), pivot_(coord_) {}

    BPDynPointLight( GFXFactory factory, GFXStorage& storage,
        const BPPointLightDesc& lightDesc
    ) : coord_(), base_(std::move(factory), storage, lightDesc),
        pivot_(coord_) {}

    void sync(GFXStorage& storage) {
        base_.sync(storage);
    }

    void config(GFXFactory factory) {
        base_.config(std::move(factory));
    }

    const BPPointLightDesc& lightDesc() const noexcept {
        return base_.lightDesc();
    }

    const dx::XMVECTOR VCALL color() const noexcept {
        return base_.color();
    }

    const float attConst() const noexcept {
        return base_.attConst();
    }

    const float attLin() const noexcept {
        return base_.attLin();
    }

    const float attQuad() const noexcept {
        return base_.attQuad();
    }

    void VCALL setPos(dx::FXMVECTOR posVal) {
        base_.setPos(posVal);
    }

    void VCALL setColor(dx::FXMVECTOR colorVal) {
        base_.setColor(colorVal);
    }

    void setAttConst(float attConstVal) {
        base_.setAttConst(attConstVal);
    }

    void setAttLin(float attLinVal) {
        base_.setAttLin(attLinVal);
    }

    void setAttQuad(float attQuadVal) {
        base_.setAttQuad(attQuadVal);
    }

    UINT slot() const {
        return base_.slot();
    }

    void setSlot(UINT val) {
        base_.setSlot(val);
    }

    CoordSystem& coordSystem() noexcept {
        return coord_;
    }

    const CoordSystem& coordSystem() const noexcept {
        return coord_;
    }

    CoordPt& pos() noexcept {
        return pivot_;
    }

    const CoordPt& pos() const noexcept {
        return pivot_;
    }

    void sync(const CameraVision& vision) {
        pVision_ = &vision;
    }

private:
    void bind(GFXPipeline& pipeline) override {
        if (!pVision_.has_value()) {
            throw GFX_EXCEPT_CUSTOM( "Utilized::BPDynPointLight must be synchronized"
                "with CameraVision when it is going to be bound."
            );
        }

        pivot_.update();
        base_.setPos( dx::XMVector3Transform(
            pivot_.abs(), pVision_.value()->viewTrans().get()
        ) );

        base_.bind(pipeline);
    }

    CoordSystem coord_;
    Basic::BPDynPointLight base_;
    CoordPt pivot_;
    std::optional<const CameraVision*> pVision_;
};

} // namespace Utilized

using namespace Utilized;


#include "Entity.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "RCDrawComponent.hpp"
#include "TransformDrawContexts.hpp"

#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/PipelineObjects/Viewport.hpp"
#include "GFX/PipelineObjects/Topology.hpp"
#include "GFX/PipelineObjects/DrawCaller.hpp"
#include "GFX/Primitives/Sphere.hpp"

#include <tuple>

class Luminance : public IEntity {
public:
    friend class Loader<Luminance>;

    Luminance(GFXFactory factory, GFXStorage& storage)
        : res_( GFXMappedResource::Type<BPDynPointLight>{}, storage,
            std::move(factory), storage
        ) {}

    void sync(const Renderer& renderer) {
        if (typeid(renderer) == typeid(BPhongRenderer)) {
            sync( static_cast<const BPhongRenderer&>(renderer) );
        }
        else {
            throw GFX_EXCEPT_CUSTOM(
                "Luminance tried to synchronize with"
                "uncompatible renderer.\n"
            );
        }
    }

    void sync(const BPhongRenderer& renderer) {
        assert(res_.valid());
        res_.as<BPDynPointLight>().setSlot(
            BPhongRenderer::slotLightCBuffer()
        );
    }

    GFXMappedResource& res() noexcept {
        return res_;
    }

    const GFXMappedResource& res() const noexcept {
        return res_;
    }

    void update(milliseconds elasped) override {}

    Loader<Luminance> loader() noexcept;

private:

    GFXMappedResource res_;
};

template <>
class Loader<Luminance> {
public:
    Loader(Luminance& entity)
        : entity_(entity) {}

    void loadAt(LSceneAdapter scene) {
        scene.addLight(entity_.res());
        entity_.res().as<BPDynPointLight>().sync(scene.vision());
    }
    void loadAt(const CoordSystem& coord) {
        entity_.res().as<BPDynPointLight>()
            .coordSystem().setParent(coord);
    }

private:
    Luminance& entity_;
};

inline Loader<Luminance> Luminance::loader() noexcept {
    return Loader<Luminance>(*this);
}

class LightViz : public IEntity {
public:
    friend class Loader<LightViz>;

    LightViz( GFXFactory factory, GFXStorage& storage,
        const Win32::Client& client
    ) : dc_( std::move(factory), storage, client ),
        base_(nullptr) {}

    LightViz( const Luminance& base, GFXFactory factory,
        GFXStorage& storage, const Win32::Client& client
    ) : dc_( std::move(factory), storage, client ),
        base_(&base) {}

    void setBase(const Luminance& base) {
        base_ = &base;
    }

    void update(milliseconds elasped) override {
        dc_.update( base_->res().as<BPDynPointLight>().coordSystem().total() );
    }

    Loader<LightViz> loader() noexcept;

private:
    class DrawComponentLViz : public RCDrawCmp {
    public:
        class MyVertexBuffer : public Primitives::Sphere::SphereVertexBuffer {
        public:
            MyVertexBuffer(GFXFactory factory)
                : Primitives::Sphere::SphereVertexBuffer(
                    std::move(factory), 32u, 32u
                ) {}
        };

        class MyIndexBuffer : public Primitives::Sphere::SphereIndexBuffer {
        public:
            MyIndexBuffer(GFXFactory factory)
                : Primitives::Sphere::SphereIndexBuffer(
                    std::move(factory), 32u, 32u
                ) {}

            static std::size_t size() noexcept {
                return Primitives::Sphere::SphereIndexBuffer::size(32u, 32u);
            }
        };

        class MyColorCBuf : public PSCBuffer<dx::XMVECTOR> {
        public:
            MyColorCBuf(GFXFactory factory)
                : PSCBuffer<dx::XMVECTOR>( std::move(factory),
                    D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE,
                    std::ranges::single_view(dx::XMVectorReplicate(1.f))
                ) {}
        };

        class MyDynColorCBuf : public IBindable {
        public:
            MyDynColorCBuf(GFXFactory factory, GFXStorage& storage)
                : colorCBuf_( GFXMappedResource::Type<MyColorCBuf>{},
                    typeid(MyColorCBuf), storage, std::move(factory) ),
                color_( dx::XMVectorReplicate(1.f) ) {
            }

            void setSlot(UINT slot) {
                colorCBuf_.as<MyColorCBuf>().setSlot(slot);
            }

            UINT slot() const {
                return colorCBuf_.as<MyColorCBuf>().slot();
            }
            
        private:
            void bind(GFXPipeline& pipeline) override {
                pipeline.bind(colorCBuf_.get());
                colorCBuf_.as<MyColorCBuf>().dynamicUpdate(
                    pipeline, [this]() { return &color_; }
                );
            }

            GFXMappedResource colorCBuf_;
            dx::XMVECTOR color_;
        };

        class MyTransformCBuf : public VSCBuffer<dx::XMMATRIX>{
        public:
            MyTransformCBuf() = default;
            MyTransformCBuf(GFXFactory factory)
                : VSCBuffer<dx::XMMATRIX>(factory, D3D11_USAGE_DYNAMIC,
                    D3D11_CPU_ACCESS_WRITE,
                    std::ranges::single_view(dx::XMMatrixIdentity())
                ) {}
        };

        class MyViewport : public Viewport {
        public:
            MyViewport(const Win32::Client& client)
                : Viewport( D3D11_VIEWPORT{
                    .TopLeftX = 0.f,
                    .TopLeftY = 0.f,
                    .Width = static_cast<FLOAT>(client.width),
                    .Height = static_cast<FLOAT>(client.height),
                    .MinDepth = 0.f,
                    .MaxDepth = 1.f
                } ) {}
        };

        class MyTopology : public Topology {
        public:
            MyTopology()
                : Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}
        };

        using MyDrawCaller = DrawCallerIndexed;

        DrawComponentLViz( GFXFactory factory, GFXStorage& storage,
            const Win32::Client& client
        ) : transformGPUMapper_(storage),
            transformApplyer_(transformGPUMapper_),
        #ifdef ACTIVATE_DRAWCOMPONENT_LOG
            logComponent_(this),
        #endif
            vBuf_( GFXMappedResource::Type<MyVertexBuffer>{},
                typeid(MyVertexBuffer), storage, factory
            ), iBuf_( GFXMappedResource::Type<MyIndexBuffer>{},
            typeid(MyIndexBuffer), storage, factory
            ), colorCBuf_( GFXMappedResource::Type<MyDynColorCBuf>{},
                storage, factory, storage
            ), transCBuf_( GFXMappedResource::Type<MyTransformCBuf>{},
                typeid(MyTransformCBuf), storage, factory
            ), viewport_( GFXMappedResource::Type<MyViewport>{},
                typeid(MyViewport), storage, client
            ), topology_( GFXMappedResource::Type<MyTopology>{},
                typeid(MyTopology), storage
            ) {
            transformGPUMapper_.setTCBufID(transCBuf_.id());

            this->setDrawCaller( std::make_unique<MyDrawCaller>(
                static_cast<UINT>( MyIndexBuffer::size() ), 0u, 0
            ) );

            this->drawCaller().addDrawContext(&transformApplyer_);
            this->drawCaller().addDrawContext(&transformGPUMapper_);
        #ifdef ACTIVATE_DRAWCOMPONENT_LOG
            logComponent_.entryStackPop();
        #endif
        }

        DrawComponentLViz(DrawComponentLViz&& other) noexcept
            : RCDrawCmp(std::move(other)),
            transformGPUMapper_( std::move(other.transformGPUMapper_) ),
            transformApplyer_( std::move(other.transformApplyer_) ),
        #ifdef ACTIVATE_DRAWCOMPONENT_LOG
            logComponent_( std::move(other.logComponent_) ),
        #endif
            vBuf_( std::move(other.vBuf_) ),
            iBuf_( std::move(other.iBuf_) ),
            colorCBuf_( std::move(other.colorCBuf_) ),
            transCBuf_( std::move(other.transCBuf_) ),
            viewport_( std::move(other.viewport_) ),
            topology_( std::move(other.topology_) ) {

            transformApplyer_.linkMapper(transformGPUMapper_);

            this->setDrawCaller( std::make_unique<MyDrawCaller>(
                static_cast<UINT>( MyIndexBuffer::size() ), 0u, 0
            ) );

            this->drawCaller().addDrawContext(&transformApplyer_);
            this->drawCaller().addDrawContext(&transformGPUMapper_);

        #ifdef ACTIVATE_DRAWCOMPONENT_LOG
            logComponent_.setLogSrc(this);
        #endif
        }

        DrawComponentLViz& operator=(DrawComponentLViz&& other) noexcept {
            other.swap(*this);
            return *this;
        }

        void update(const Transform transform) {
            transformGPUMapper_.update(transform);
        }

        void sync(const Renderer& renderer) override {
            if (typeid(renderer) == typeid(SolidRenderer)) {
                sync(static_cast<const SolidRenderer&>(renderer));
            }
            else {
                throw GFX_EXCEPT_CUSTOM("PointLight's Visualization DrawComponent"
                    "tried to synchronize with unsupported Renderer.\n"
                );
            }
        }

        void sync(const SolidRenderer& renderer) {
            assert(vBuf_.valid());
            vBuf_.as<MyVertexBuffer>().setSlot( SolidRenderer::slotPosBuffer() );

            assert(colorCBuf_.valid());
            colorCBuf_.as<MyDynColorCBuf>().setSlot( SolidRenderer::slotColorCBuf() );

            assert(transCBuf_.valid());
            transCBuf_.as<MyTransformCBuf>().setSlot( SolidRenderer::slotTransCBuf() );

            this->setRODesc( RenderObjectDesc{
                .header = RenderObjectDesc::Header{
                    .IDBuffer = vBuf_.id(),
                    .IDType = typeid(*this)
                },
                .IDs = {
                    vBuf_.id(), iBuf_.id(), colorCBuf_.id(),
                    transCBuf_.id(), viewport_.id(), topology_.id()
                }
            } );
        }

        void sync(const CameraVision& vision) override {
            transformApplyer_.setTransform(
                vision.viewTrans() * vision.projTrans()
            );
        }

        auto reflect() noexcept {
            return std::tie( transformGPUMapper_, transformApplyer_,
                logComponent_, vBuf_, iBuf_, colorCBuf_, transCBuf_,
                viewport_, topology_
            );
        }

        auto reflect() const noexcept {
            return std::tie( transformGPUMapper_, transformApplyer_,
                logComponent_, vBuf_, iBuf_, colorCBuf_, transCBuf_,
                viewport_, topology_
            );
        }

        void swap(DrawComponentLViz& rhs) {
            auto rhsR = rhs.reflect();
            reflect().swap(rhsR);
            transformApplyer_.linkMapper(transformGPUMapper_);
            rhs.transformApplyer_.linkMapper(rhs.transformGPUMapper_);
        #ifdef ACTIVATE_DRAWCOMPONENT_LOG
            logComponent_.setLogSrc(this);
            rhs.logComponent_.setLogSrc(&rhs);
        #endif
        }

    private:
        MapTransformGPU transformGPUMapper_;
        ApplyTransform transformApplyer_;
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        RCDrawCmp::LogComponent logComponent_;
    #endif
        GFXMappedResource vBuf_;
        GFXMappedResource iBuf_;
        GFXMappedResource colorCBuf_;
        GFXMappedResource transCBuf_;
        GFXMappedResource viewport_;
        GFXMappedResource topology_;
    };  // class DrawComponentLViz

    DrawComponentLViz& dc() noexcept {
        return dc_;
    }

    const DrawComponentLViz& dc() const noexcept {
        return dc_;
    }

    DrawComponentLViz dc_;
    const Luminance* base_;
};

template <>
class Loader<LightViz> {
public:
    Loader(LightViz& entity)
        : entity_(entity) {}

    void loadAt(Layer& layer) {
        layer.addDrawCmp(&entity_.dc());
    }

    void loadAt(Scene& scene, std::size_t layerIdx) {
        loadAt(scene.layer(layerIdx));
    }

private:
    LightViz& entity_;
};

inline Loader<LightViz> LightViz::loader() noexcept {
    return Loader<LightViz>(*this);
}

class LightEntity : public IEntity {
public:
    void update(milliseconds elapsed) override {
        if (luminance_.has_value()) [[likely]] {
            luminance_.value().update(elapsed);
        }
        if (viz_.has_value()) {
            viz_.value().update(elapsed);
        }
    }

    void ctLuminance(GFXFactory factory, GFXStorage& storage) {
        luminance_ = Luminance(std::move(factory), storage);
        if (viz_.has_value()) {
            viz_.value().setBase(luminance_.value());
        }
    }

    void ctViz( GFXFactory factory, GFXStorage& storage,
        const Win32::Client& client
    ) {
        if (luminance_.has_value()) {
            viz_ = LightViz( luminance_.value(),
                std::move(factory), storage, client
            );
        }
        else {
            viz_ = LightViz(std::move(factory), storage, client);
        }
    }

    Luminance& luminance() noexcept {
        assert(luminance_.has_value());
        return luminance_.value();
    }

    const Luminance& luminance() const noexcept {
        assert(luminance_.has_value());
        return luminance_.value();
    }

    LightViz& viz() noexcept {
        assert(viz_.has_value());
        return viz_.value();
    }

    const LightViz& viz() const noexcept {
        assert(viz_.has_value());
        return viz_.value();
    }

private:
    std::optional<Luminance> luminance_;
    std::optional<LightViz> viz_;
};

#endif  // __PointLight