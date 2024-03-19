#ifndef __Texture
#define __Texture

#include "PipelineObject.hpp"
#include "GFX/Core/Factory.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <d3d11.h>
#include "GFX/Core/Namespaces.hpp"
#include "GFX/Core/Exception.hpp"

#include "Image/Surface.hpp"

namespace gfx {
namespace po {

class TextureBinder : public SlotBinderInterface<TextureBinder> {
public:
    friend class SlotBinderInterface<TextureBinder>; 

private:
    void doBind( GFXPipeline& pipeline, UINT slot,
        ID3D11ShaderResourceView** srv
    );
};

class Texture : public IPipelineObject,
    SlotLocalRebindInterface<Texture> {
public:
    friend class SlotLocalRebindInterface<Texture>; 

    Texture( GFXFactory factory, const Surface& surface
    #ifdef ACTIVATE_BINDABLE_LOG
        , bool enableLogOnCreation = true
    #endif
    );

    UINT slot() const noexcept {
        return slot_;
    }

    void setSlot(UINT val) noexcept {
        slot_ = val;
    }

private:
    void bind(GFXPipeline& pipeline) override final;

#ifdef ACTIVATE_BINDABLE_LOG
    IPipelineObject::LogComponent logComponent_;
#endif
    wrl::ComPtr<ID3D11ShaderResourceView> pSRV_;
    UINT slot_;
    TextureBinder binder_;
};

}   // namespace gfx
}   // namespace po

#endif  // __Texture