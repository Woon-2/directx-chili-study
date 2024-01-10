#ifndef __PointLight
#define __PointLight

#include "GFX/PipelineObjects/Bindable.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/GraphicsStorage.hpp"

#include "GFX/Core/GraphicsNamespaces.hpp"

#include <ranges>

struct BPPointLightDesc {
    dx::XMFLOAT3A pos;
    dx::XMFLOAT3 color;
    float attConst;
    float attLin;
    float attQuad;
};

class BPDynPointLight : public IBindable{
private:
    using MyPSCBuffer = PSCBuffer<BPPointLightDesc>;

public:
    BPDynPointLight()
        : BPDynPointLight( defLightDesc() ) {}

    BPDynPointLight(const BPPointLightDesc& lightDesc)
        : lightDesc_(lightDesc), res_(), dirty_(false) {}

    BPDynPointLight(GFXFactory& factory, GFXStorage& storage)
        : BPDynPointLight( factory, storage, defLightDesc() ) {}

    BPDynPointLight(GFXFactory& factory)
        : BPDynPointLight( factory, defLightDesc() ) {}

    BPDynPointLight(GFXFactory& factory, const BPPointLightDesc& lightDesc)
        : lightDesc_(lightDesc), res_(), dirty_(false) {
        config(factory);
    }

    BPDynPointLight( GFXFactory& factory, GFXStorage& storage,
        const BPPointLightDesc& lightDesc
    ) : lightDesc_(lightDesc),
        res_( GFXMappedResource::Type<MyPSCBuffer>{}, storage,
            // the resource construction arguments follow.
            factory, D3D11_USAGE_DEFAULT, 0,
            std::ranges::single_view(lightDesc_)
        ), dirty_(false) {}

    void sync(GFXStorage& storage) {
        res_.sync(storage);
        res_.remap();
    }

    void config(GFXFactory& factory) {
        res_.config<MyPSCBuffer>( factory, D3D11_USAGE_DEFAULT, 0,
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

    void VCALL setPos(dx::XMVECTOR posVal) {
        dx::XMStoreFloat3A(&lightDesc_.pos, posVal);
        dirty_ = true;
    }

    void VCALL setColor(dx::XMVECTOR colorVal) {
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
        res.enableLocalRebindTemporary();
    }

private:
    // referenced attenuation coefficient constants from
    // https://wiki.ogre3d.org/-Point+Light+Attenuation
    static const BPPointLightDesc defLightDesc() noexcept {
        return BPPointLightDesc{
            .pos = dx::XMFLOAT3A(0.f, 0.f, 0.f),
            .color = dx::XMFLOAT3(1.f, 1.f, 1.f),
            .attConst = 1.f,
            .attLin = 0.045f,
            .attQuad = 0.0075f
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

#endif  // __PointLight