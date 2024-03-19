#ifndef __PointLightControl
#define __PointLightControl

#include "GFX/Scenery/PointLight.hpp"

#include "GFX/Core/Namespaces.hpp"

class PointLightControlBase {
public:
    PointLightControlBase(bool willShow = true)
        : PointLightControlBase(
            gfx::scenery::Basic::BPDynPointLight::defLightDesc(),
            willShow
        ) {}

    PointLightControlBase( const gfx::scenery::BPPointLightDesc& desc,
        bool willShow = true
    );

    void render();

    bool shown() const noexcept {
        return willShow_;
    }

    void show() noexcept {
        willShow_ = true;
    }

    void close() noexcept {
        willShow_ = false;
    }

    void reset() noexcept;

protected:
    gfx::scenery::BPPointLightDesc initialData_;
    dx::XMVECTOR pos_;
    dx::XMVECTOR color_;
    float attConst_;
    float attLin_;
    float attQuad_;
    bool willShow_;
};

namespace Basic {

class PointLightControl : public PointLightControlBase {
public:
    PointLightControl(bool willShow = true)
        : PointLightControlBase(willShow) {}

    PointLightControl( const gfx::scenery::BPPointLightDesc& desc,
        bool willShow = true
    ) : PointLightControlBase(desc, willShow) {}

    void submit(gfx::scenery::Basic::BPDynPointLight& lum);
    void submit(gfx::scenery::Luminance& lum);
    void submit(gfx::scenery::LightEntity& light);
};

}  // namespace Basic

namespace Utilized {

class PointLightControl : public PointLightControlBase {
public:
    PointLightControl(bool willShow = true)
        : PointLightControlBase(willShow) {}

    PointLightControl( const gfx::scenery::BPPointLightDesc& desc,
        bool willShow = true
    ) : PointLightControlBase(desc, willShow) {}

    void submit(gfx::scenery::Utilized::BPDynPointLight& lum);
    void submit(gfx::scenery::Luminance& lum);
    void submit(gfx::scenery::LightEntity& light);
};

}  // namespace Utilized

using namespace Utilized;

#endif  // __PointLightControl