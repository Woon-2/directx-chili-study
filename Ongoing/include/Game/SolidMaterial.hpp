#ifndef __SolidMaterial
#define __SolidMaterial

#include "GFX/PipelineObjects/Bindable.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/GraphicsStorage.hpp"

#include "GFX/Core/GraphicsNamespaces.hpp"

struct SolidMaterialDesc {
    dx::XMFLOAT3A diffuse;
    dx::XMFLOAT3 specular;
    float shinyness;
    dx::XMFLOAT3A ambient;
    dx::XMFLOAT3A emmisive;
};

class SolidMaterial : public IBindable{
private:
    using MyPSCBuffer = PSCBuffer<SolidMaterialDesc>;

public:
    SolidMaterial()
        : SolidMaterial(defMatDesc()) {}

    SolidMaterial(const SolidMaterialDesc& matDesc)
        : matDesc_(matDesc), res_() {}

    SolidMaterial(GFXFactory factory, GFXStorage& storage)
        : SolidMaterial( std::move(factory), storage, defMatDesc() ) {}

    SolidMaterial(GFXFactory factory)
        : SolidMaterial( std::move(factory), defMatDesc() ) {}

    SolidMaterial(GFXFactory factory, const SolidMaterialDesc& matDesc)
        : matDesc_(matDesc), res_() {
        config(std::move(factory));        
    }

    SolidMaterial( GFXFactory factory, GFXStorage& storage,
        const SolidMaterialDesc& matDesc
    ) : matDesc_(matDesc),
    res_( GFXMappedResource::Type<MyPSCBuffer>{},
        typeid(MyPSCBuffer), storage,
        std::move(factory), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE,
        std::ranges::single_view(matDesc_)
    ) {}

    void sync(GFXStorage& storage) {
        res_.sync(storage);
        res_.remap();
    }

    void config(GFXFactory factory) {
        res_.config<MyPSCBuffer>( std::move(factory), D3D11_USAGE_DEFAULT, 0,
            std::ranges::single_view(matDesc_)
        );
    }

    const SolidMaterialDesc& matDesc() const noexcept {
        return matDesc_;
    }

    const dx::XMVECTOR VCALL diffuse() const noexcept {
        return dx::XMLoadFloat3A(&matDesc_.diffuse);
    }

    const dx::XMVECTOR VCALL specular() const noexcept {
        return dx::XMLoadFloat3(&matDesc_.specular);
    }

    const float shinyness() const noexcept {
        return matDesc_.shinyness;
    }

    const dx::XMVECTOR VCALL ambient() const noexcept {
        return dx::XMLoadFloat3A(&matDesc_.ambient);
    }

    const dx::XMVECTOR VCALL emmisive() const noexcept {
        return dx::XMLoadFloat3A(&matDesc_.emmisive);
    }

    void VCALL setDiffuse(dx::FXMVECTOR color) {
        dx::XMStoreFloat3A(&matDesc_.diffuse, color);
    }

    void VCALL setSpecular(dx::FXMVECTOR color) {
        dx::XMStoreFloat3(&matDesc_.specular, color);
    }

    void setShyniness(float val) {
        matDesc_.shinyness = val;
    }

    void VCALL setAmbient(dx::FXMVECTOR color) {
        dx::XMStoreFloat3A(&matDesc_.ambient, color);
    }

    void VCALL setEmmisive(dx::FXMVECTOR color) {
        dx::XMStoreFloat3A(&matDesc_.emmisive, color);
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
    static const SolidMaterialDesc defMatDesc() noexcept {
        return SolidMaterialDesc{
            .diffuse = dx::XMFLOAT3A(0.5f, 0.5f, 0.5f),
            .specular = dx::XMFLOAT3(0.1f, 0.1f, 0.1f),
            .shinyness = 4.f,
            .ambient = dx::XMFLOAT3A(0.f, 0.f, 0.f),
            .emmisive = dx::XMFLOAT3A(0.f, 0.f, 0.f)
        };
    }

    void bind(GFXPipeline& pipeline) override {
        // if rebind required, rebind
        // it is handled in PSCBuffer internally
        pipeline.bind(res_.get());

        res_.as<MyPSCBuffer>().dynamicUpdate( pipeline, [this](){
            return &matDesc_;
        } );
    }

    SolidMaterialDesc matDesc_;
    // res_ depends on matDesc_ in initialization.
    // so res_ should be here.
    GFXMappedResource res_;
};

#endif  // __SolidMaterial