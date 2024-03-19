#include "GFX/Scenery/SolidMaterial.hpp"

namespace gfx {
namespace scenery {

SolidMaterial::SolidMaterial(GFXFactory factory, GFXStorage& storage)
    : SolidMaterial( std::move(factory), storage, defMatDesc() ) {}

SolidMaterial::SolidMaterial( GFXFactory factory, GFXStorage& storage,
    const SolidMaterialDesc& matDesc
) : matDesc_(matDesc),
    cbuf_( std::move(factory), D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE,
        std::ranges::single_view(matDesc_)
    ) {}

const SolidMaterialDesc SolidMaterial::defMatDesc() noexcept {
    return SolidMaterialDesc{
        .diffuse = dx::XMFLOAT3A(0.6f, 0.6f, 0.6f),
        .specular = dx::XMFLOAT3(0.6f, 0.6f, 0.6f),
        .shinyness = 51.2f,
        .ambient = dx::XMFLOAT3A(0.f, 0.f, 0.f),
        .emmisive = dx::XMFLOAT3A(0.f, 0.f, 0.f)
    };
}

void SolidMaterial::bind(GFXPipeline& pipeline) /* overriden */ {
    // if rebind required, rebind
    // it is handled in PSCBuffer internally
    pipeline.bind(&cbuf_);

    cbuf_.dynamicUpdate( pipeline, [this](){ return &matDesc_; } );
}

}   // namespace gfx::scenery
}   // namespace gfx