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

#endif  // __PointLight