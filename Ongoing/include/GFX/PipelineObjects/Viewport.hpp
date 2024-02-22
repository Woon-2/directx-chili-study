#ifndef __Viewport
#define __Viewport

#include "Bindable.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <d3d11.h>
#include "GFX/Core/Namespaces.hpp"
#include "GFX/Core/Exception.hpp"

class ViewportBinder : public BinderInterface<ViewportBinder> {
public:
    friend class BinderInterface<ViewportBinder>;

private:
    void doBind(GFXPipeline& pipeline, D3D11_VIEWPORT* pViewport) {
        GFX_THROW_FAILED_VOID(
            pipeline.context()->RSSetViewports(1u, pViewport)
        );
    }
};

class Viewport : public IBindable,
    public LocalRebindInterface<Viewport> {
public:
    friend class LocalRebindInterface<Viewport>;

    Viewport( const D3D11_VIEWPORT& data
    #ifdef ACTIVATE_BINDABLE_LOG
        , bool enableLogOnCreation = true
    #endif
    ) :
    #ifdef ACTIVATE_BINDABLE_LOG
        logComponent_( this, GFXCMDSourceCategory("Topology") ),
    #endif
        data_(data) {
    #ifdef ACTIVATE_BINDABLE_LOG
        if (enableLogOnCreation) {
            logComponent_.enableLog();
        }
        logComponent_.logCreate();
    #endif
    }

    Viewport( D3D11_VIEWPORT&& data
    #ifdef ACTIVATE_BINDABLE_LOG
        , bool enableLogOnCreation = true
    #endif
    ) :
    #ifdef ACTIVATE_BINDABLE_LOG
        logComponent_( this, GFXCMDSourceCategory("Topology") ),
    #endif
        data_(std::move(data)) {
    #ifdef ACTIVATE_BINDABLE_LOG
        if (enableLogOnCreation) {
            logComponent_.enableLog();
        }
        logComponent_.logCreate();
    #endif
    }

    D3D11_VIEWPORT* data() noexcept {
        return &data_;
    }

    const D3D11_VIEWPORT* data() const noexcept {
        return &data_;
    }

private:
    void bind(GFXPipeline& pipeline) override final {
        [[maybe_unused]] auto bBindOccured = binder_.bind(
            pipeline, data()
        );

    #ifdef ACTIVATE_BINDABLE_LOG
        if (bBindOccured) {
            logComponent_.logBind();
        }
    #endif 
    }

#ifdef ACTIVATE_BINDABLE_LOG
    IBindable::LogComponent logComponent_;
#endif
    D3D11_VIEWPORT data_;
    ViewportBinder binder_;
};

#endif  // __Viewport