#ifndef __Viewport
#define __Viewport

#include "Bindable.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <d3d11.h>
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

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

    Viewport(const D3D11_VIEWPORT& data)
        : data_(data) {}

    Viewport(D3D11_VIEWPORT&& data)
        : data_(std::move(data)) {}

    D3D11_VIEWPORT* data() noexcept {
        return &data_;
    }

    const D3D11_VIEWPORT* data() const noexcept {
        return &data_;
    }

private:
    void bind(GFXPipeline& pipeline) override final {
        binder_.bind( pipeline, data() );
    }

    D3D11_VIEWPORT data_;
    ViewportBinder binder_;
};

#endif  // __Viewport