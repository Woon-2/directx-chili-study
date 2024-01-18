#include "Game/PointLight.hpp"

namespace Basic {

BPDynPointLight::BPDynPointLight()
    : BPDynPointLight( defLightDesc() ) {}

BPDynPointLight::BPDynPointLight(const BPPointLightDesc& lightDesc)
    : lightDesc_(lightDesc), res_(), dirty_(false) {}

BPDynPointLight::BPDynPointLight(GFXFactory factory, GFXStorage& storage)
    : BPDynPointLight( std::move(factory), storage, defLightDesc() ) {}

BPDynPointLight::BPDynPointLight(GFXFactory factory)
    : BPDynPointLight( std::move(factory), defLightDesc() ) {}

BPDynPointLight::BPDynPointLight(GFXFactory factory, const BPPointLightDesc& lightDesc)
    : lightDesc_(lightDesc), res_(), dirty_(false) {
    config(std::move(factory));
}

BPDynPointLight::BPDynPointLight( GFXFactory factory, GFXStorage& storage,
    const BPPointLightDesc& lightDesc
) : lightDesc_(lightDesc),
    res_( GFXMappedResource::Type<MyPSCBuffer>{}, storage,
        // the resource construction arguments follow.
        std::move(factory), D3D11_USAGE_DEFAULT, 0,
        std::ranges::single_view(lightDesc_)
    ), dirty_(false) {}

void BPDynPointLight::sync(GFXStorage& storage) {
    res_.sync(storage);
    res_.remap();
}

void BPDynPointLight::config(GFXFactory factory) {
    res_.config<MyPSCBuffer>( std::move(factory), D3D11_USAGE_DEFAULT, 0,
        std::ranges::single_view(lightDesc_)
    );
}

void BPDynPointLight::bind(GFXPipeline& pipeline) /* overriden */ {
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

}   // namespace Basic

namespace Utilized {

BPDynPointLight::BPDynPointLight()
    : coord_(), base_(), pivot_(coord_) {}

BPDynPointLight::BPDynPointLight(const BPPointLightDesc& lightDesc)
    : coord_(), base_(lightDesc), pivot_(coord_) {}

BPDynPointLight::BPDynPointLight(GFXFactory factory, GFXStorage& storage)
    : coord_(), base_(std::move(factory), storage), pivot_(coord_) {}

BPDynPointLight::BPDynPointLight(GFXFactory factory)
    : coord_(), base_(std::move(factory)), pivot_(coord_) {}

BPDynPointLight::BPDynPointLight(GFXFactory factory, const BPPointLightDesc& lightDesc)
    : coord_(), base_(std::move(factory), lightDesc), pivot_(coord_) {}

BPDynPointLight::BPDynPointLight( GFXFactory factory, GFXStorage& storage,
    const BPPointLightDesc& lightDesc
) : coord_(), base_(std::move(factory), storage, lightDesc),
    pivot_(coord_) {}

void BPDynPointLight::bind(GFXPipeline& pipeline) /* overriden */ {
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

}   // namespace Utilized

Luminance::Luminance(GFXFactory factory, GFXStorage& storage)
    : res_( GFXMappedResource::Type<BPDynPointLight>{}, storage,
        std::move(factory), storage
    ) {}

void Luminance::sync(const Renderer& renderer) {
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

void Luminance::sync(const BPhongRenderer& renderer) {
    assert(res_.valid());
    res_.as<BPDynPointLight>().setSlot(
        BPhongRenderer::slotLightCBuffer()
    );
}

LightViz::LightViz( GFXFactory factory, GFXStorage& storage,
    const Win32::Client& client
) : dc_( std::move(factory), storage, client ),
    base_(nullptr) {}

LightViz::LightViz( const Luminance& base, GFXFactory factory,
    GFXStorage& storage, const Win32::Client& client
) : dc_( std::move(factory), storage, client ),
    base_(&base) {}

class LightViz::DrawComponentLViz::MyVertexBuffer
    : public Primitives::Sphere::SphereVertexBuffer {
public:
    MyVertexBuffer(GFXFactory factory)
        : Primitives::Sphere::SphereVertexBuffer(
            std::move(factory), 32u, 32u
        ) {}
};

class LightViz::DrawComponentLViz::MyIndexBuffer
    : public Primitives::Sphere::SphereIndexBuffer {
public:
    MyIndexBuffer(GFXFactory factory)
        : Primitives::Sphere::SphereIndexBuffer(
            std::move(factory), 32u, 32u
        ) {}

    static std::size_t size() noexcept {
        return Primitives::Sphere::SphereIndexBuffer::size(32u, 32u);
    }
};

class LightViz::DrawComponentLViz::MyColorCBuf
    : public PSCBuffer<dx::XMVECTOR> {
public:
    MyColorCBuf(GFXFactory factory)
        : PSCBuffer<dx::XMVECTOR>( std::move(factory),
            D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE,
            std::ranges::single_view(dx::XMVectorReplicate(1.f))
        ) {}
};

class LightViz::DrawComponentLViz::MyDynColorCBuf
    : public IBindable {
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

class LightViz::DrawComponentLViz::MyTransformCBuf
    : public VSCBuffer<dx::XMMATRIX>{
public:
    MyTransformCBuf() = default;
    MyTransformCBuf(GFXFactory factory)
        : VSCBuffer<dx::XMMATRIX>(factory, D3D11_USAGE_DYNAMIC,
            D3D11_CPU_ACCESS_WRITE,
            std::ranges::single_view(dx::XMMatrixIdentity())
        ) {}
};

class LightViz::DrawComponentLViz::MyViewport
    : public Viewport {
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

class LightViz::DrawComponentLViz::MyTopology
    : public Topology {
public:
    MyTopology()
        : Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}
};

LightViz::DrawComponentLViz::DrawComponentLViz(
    GFXFactory factory, GFXStorage& storage,
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

LightViz::DrawComponentLViz::DrawComponentLViz(
    DrawComponentLViz&& other
) noexcept
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

LightViz::DrawComponentLViz&
LightViz::DrawComponentLViz::operator=(DrawComponentLViz&& other) noexcept {
    other.swap(*this);
    return *this;
}

void LightViz::DrawComponentLViz::sync(
    const Renderer& renderer
) /* overriden */ {
    if (typeid(renderer) == typeid(SolidRenderer)) {
        sync(static_cast<const SolidRenderer&>(renderer));
    }
    else {
        throw GFX_EXCEPT_CUSTOM("PointLight's Visualization DrawComponent"
            "tried to synchronize with unsupported Renderer.\n"
        );
    }
}

void LightViz::DrawComponentLViz::sync(const SolidRenderer& renderer) {
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

void LightViz::DrawComponentLViz::sync(
    const CameraVision& vision
) /* overriden */ {
    transformApplyer_.setTransform(
        vision.viewTrans() * vision.projTrans()
    );
}

void LightViz::DrawComponentLViz::swap(
    DrawComponentLViz& rhs
) {
    auto rhsR = rhs.reflect();
    reflect().swap(rhsR);
    transformApplyer_.linkMapper(transformGPUMapper_);
    rhs.transformApplyer_.linkMapper(rhs.transformGPUMapper_);
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
    logComponent_.setLogSrc(this);
    rhs.logComponent_.setLogSrc(&rhs);
#endif
}