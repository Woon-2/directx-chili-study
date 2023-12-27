#ifndef __Sampler
#define __Sampler

#include "Bindable.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <d3d11.h>
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

class SamplerBinder : public BinderInterface<SamplerBinder> {
public:
    friend class BinderInterface<SamplerBinder>;

private:
    void doBind( GFXPipeline& pipeline, UINT slot, 
        ID3D11SamplerState* pSampler
    );
};

class Sampler : public IBindable,
    LocalRebindInterface<Sampler> {
public:
    friend class LocalRebindInterface<Sampler>;

    Sampler( GFXFactory factory
    #ifdef ACTIVATE_BINDABLE_LOG
        , bool enableLogOnCreation = true
    #endif
    );

    UINT slot() const noexcept {
        return slot_;
    }

    void setSlot(UINT val) noexcept {
        if (val != slot_) {
            binder_.enableLocalRebindTemporary();
        }
        slot_ = val;
    }

private:
    void bind(GFXPipeline& pipeline) override final;

#ifdef ACTIVATE_BINDABLE_LOG
    IBindable::LogComponent logComponent_;
#endif
    wrl::ComPtr<ID3D11SamplerState> pSampler_;
    UINT slot_;
    SamplerBinder binder_;
};

#endif  // __Sampler