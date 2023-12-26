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
    void doBind( GFXPipeline& pipeline, std::size_t slot,
        ID3D11ShaderResourceView** srv
    );
};

class Texture : public IBindable,
    LocalRebindInterface<Texture> {
public:
    friend class LocalRebindInterface<Texture>; 

    Texture( GFXFactory factory, std::size_t slot,
        const Surface& surface
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
    IBindable::LogComponent logComponent_;
#endif
    wrl::ComPtr<ID3D11ShaderResourceView> pSRV_;
    std::size_t slot_;
    TextureBinder binder_;
};

#endif  // __Texture