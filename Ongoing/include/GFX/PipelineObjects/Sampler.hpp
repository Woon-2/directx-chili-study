#ifndef __Sampler
#define __Sampler

#include "PipelineObject.hpp"
#include "GFX/Core/Factory.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <d3d11.h>
#include "GFX/Core/Namespaces.hpp"
#include "GFX/Core/Exception.hpp"

namespace gfx {
namespace po {

class SamplerBinder : public SlotBinderInterface<SamplerBinder> {
public:
    friend class SlotBinderInterface<SamplerBinder>;

private:
    void doBind( GFXPipeline& pipeline, UINT slot, 
        ID3D11SamplerState* pSampler
    );
};

class Sampler : public IPipelineObject,
    SlotLocalRebindInterface<Sampler> {
public:
    friend class SlotLocalRebindInterface<Sampler>;

    Sampler( GFXFactory factory
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
    wrl::ComPtr<ID3D11SamplerState> pSampler_;
    UINT slot_;
    SamplerBinder binder_;
};

}   // namespace gfx::po
}   // namespace gfx

#endif  // __Sampler