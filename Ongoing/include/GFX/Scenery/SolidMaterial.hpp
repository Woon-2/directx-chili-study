#ifndef __SolidMaterial
#define __SolidMaterial

#include "GFX/PipelineObjects/PipelineObject.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/Core/Factory.hpp"
#include "GFX/Core/Storage.hpp"

#include "GFX/Core/Namespaces.hpp"

namespace gfx {
namespace scenery {

struct SolidMaterialDesc {
    dx::XMFLOAT3A diffuse;
    dx::XMFLOAT3 specular;
    float shinyness;
    dx::XMFLOAT3A ambient;
    dx::XMFLOAT3A emmisive;
};

class SolidMaterial : public po::IPipelineObject {
private:
    using MyPSCBuffer = po::PSCBuffer<SolidMaterialDesc>;

public:
    SolidMaterial() = default;
    SolidMaterial(GFXFactory factory, GFXStorage& storage);
    SolidMaterial( GFXFactory factory, GFXStorage& storage,
        const SolidMaterialDesc& matDesc
    );

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

    void setShinyness(float val) {
        matDesc_.shinyness = val;
    }

    void VCALL setAmbient(dx::FXMVECTOR color) {
        dx::XMStoreFloat3A(&matDesc_.ambient, color);
    }

    void VCALL setEmmisive(dx::FXMVECTOR color) {
        dx::XMStoreFloat3A(&matDesc_.emmisive, color);
    }

    UINT slot() const {
        return cbuf_.slot();
    }

    void setSlot(UINT val) {
        cbuf_.setSlot(val);
    }

private:
    static const SolidMaterialDesc defMatDesc() noexcept;

    void bind(GFXPipeline& pipeline) override;

    SolidMaterialDesc matDesc_;
    MyPSCBuffer cbuf_;
};

}  // namespace scenery
}  // namespace gfx

#endif  // __SolidMaterial