#ifndef __Viewport
#define __Viewport

#include "Bindable.hpp"

#include "GFX/Core/Pipeline.hpp"
#include <d3d11.h>
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

class Viewport : public IBindable {
public:
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
    void bind(GFXPipeline& pipeline) override {
        GFX_THROW_FAILED_VOID(
            pipeline.context()->RSSetViewports(1u, data())
        );
    }

    D3D11_VIEWPORT data_;
};

#endif  // __Viewport