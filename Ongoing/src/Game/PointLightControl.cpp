#include "Game/PointLightControl.hpp"
#include "imgui.h"

PointLightControlBase::PointLightControlBase(
    const gfx::scenery::BPPointLightDesc& desc, bool willShow
) : initialData_(desc),
    pos_( dx::XMLoadFloat3A(&desc.pos) ),
    color_( dx::XMLoadFloat3(&desc.color) ),
    attConst_(desc.attConst),
    attLin_(desc.attLin),
    attQuad_(desc.attQuad),
    willShow_(willShow) {}

void PointLightControlBase::reset() noexcept {
    pos_ = dx::XMLoadFloat3A(&initialData_.pos);
    color_ = dx::XMLoadFloat3(&initialData_.color);
    attConst_ = initialData_.attConst;
    attLin_ = initialData_.attLin;
    attQuad_ = initialData_.attQuad;
}

void PointLightControlBase::render() {
    if ( !shown() ) {
        return;
    }

    if( ImGui::Begin("Light", &willShow_) ) {
        ImGui::SliderFloat3( "Position",
            reinterpret_cast<float*>(&pos_), -60.f, 60.f, "%.1f"
        );
        ImGui::ColorEdit3( "Color",
            reinterpret_cast<float*>(&color_)
        );
        ImGui::Text("Falloff");
        ImGui::SliderFloat("Constant", &attConst_, 0.01f, 5.f, "%.2f");
        ImGui::SliderFloat("Linear", &attLin_, 0.0001f, 0.2f, "%.4f");
        ImGui::SliderFloat("Quadratic", &attQuad_, 0.0000001f, 0.1f, "%.7f");

        if (ImGui::Button("Reset")) {
            reset();
        }
    }

    ImGui::End();
}

namespace Basic {

void PointLightControl::submit(gfx::scenery::Basic::BPDynPointLight& lum) {
    lum.setPos(pos_);
    lum.setColor(color_);
    lum.setAttConst(attConst_);
    lum.setAttLin(attLin_);
    lum.setAttQuad(attQuad_);
}

void PointLightControl::submit(gfx::scenery::Luminance& lum) {
    assert( lum.res().valid() );
    submit( lum.res().as<gfx::scenery::Basic::BPDynPointLight>() );
}

void PointLightControl::submit(gfx::scenery::LightEntity& light) {
    if (light.hasLuminance()) {
        submit(light.luminance());
    }
}


}   // namespace Basic

namespace Utilized {

void PointLightControl::submit(gfx::scenery::Utilized::BPDynPointLight& lum) {
    lum.setPos(pos_);
    lum.setColor(color_);
    lum.setAttConst(attConst_);
    lum.setAttLin(attLin_);
    lum.setAttQuad(attQuad_);
}

void PointLightControl::submit(gfx::scenery::Luminance& lum) {
    assert( lum.res().valid() );
    submit( lum.res().as<gfx::scenery::Utilized::BPDynPointLight>() );
}

void PointLightControl::submit(gfx::scenery::LightEntity& light) {
    if (light.hasLuminance()) {
        submit(light.luminance());
    }
}

}   // namespace Utilized