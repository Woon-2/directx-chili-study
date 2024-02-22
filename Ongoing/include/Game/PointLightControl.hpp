#ifndef __PointLightControl
#define __PointLightControl

#include "PointLight.hpp"

#include "GFX/Core/Namespaces.hpp"

class PointLightControlBase {
public:
    PointLightControlBase(bool willShow = true)
        : PointLightControlBase(
            Basic::BPDynPointLight::defLightDesc(),
            willShow
        ) {}

    PointLightControlBase( const BPPointLightDesc& desc,
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
    BPPointLightDesc initialData_;
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

    PointLightControl( const BPPointLightDesc& desc,
        bool willShow = true
    ) : PointLightControlBase(desc, willShow) {}

    void submit(Basic::BPDynPointLight& lum);
    void submit(Luminance& lum);
    void submit(LightEntity& light);
};

}  // namespace Basic

namespace Utilized {

class PointLightControl : public PointLightControlBase {
public:
    PointLightControl(bool willShow = true)
        : PointLightControlBase(willShow) {}

    PointLightControl( const BPPointLightDesc& desc,
        bool willShow = true
    ) : PointLightControlBase(desc, willShow) {}

    void submit(Utilized::BPDynPointLight& lum);
    void submit(Luminance& lum);
    void submit(LightEntity& light);
};

}  // namespace Utilized

using namespace Utilized;

#endif  // __PointLightControl