#ifndef __Texture
#define __Texture

#include "Bindable.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <d3d11.h>
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

#include "Image/Surface.hpp"

class TextureBinder : public BinderInterface<TextureBinder> {
public:
    friend class BinderInterface<TextureBinder>; 

private:
    void doBind( GFXPipeline& pipeline, UINT slot,
        ID3D11ShaderResourceView** srv
    );
};

class Texture : public IBindable,
    LocalRebindInterface<Texture> {
public:
    friend class LocalRebindInterface<Texture>; 

    Texture( GFXFactory factory, const Surface& surface
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
    wrl::ComPtr<ID3D11ShaderResourceView> pSRV_;
    UINT slot_;
    TextureBinder binder_;
};

#endif  // __Texture