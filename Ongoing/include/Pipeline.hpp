#ifndef __Pipeline
#define __Pipeline

#include "Bindable.hpp"
#include "DrawContext.hpp"

#include "GraphicsNamespaces.hpp"
#include "GraphicsException.hpp"
#include <d3d11.h>

class GFXPipeline {
public:
    GFXPipeline() = default;

    GFXPipeline(wrl::ComPtr<ID3D11DeviceContext> pContext)
        : pContext_(pContext) {

    }

    void bind(IBindable* bindable) {
        bindable->bind(*this);
    }

    void drawCall(const IDrawContext* drawContext) {
        drawContext->drawCall(*this);
    }

    void setContext(wrl::ComPtr<ID3D11DeviceContext> pContext) {
        pContext_ = pContext;
    }

    wrl::ComPtr<ID3D11DeviceContext> context() const noexcept {
        return pContext_;
    }

    decltype(auto) operator&() {
        return &pContext_;
    }

    decltype(auto) operator&() const {
        return &pContext_;
    }

    GFXPipeline* address() noexcept {
        return this;
    }

    const GFXPipeline* address() const noexcept {
        return this;
    }

private:
    wrl::ComPtr<ID3D11DeviceContext> pContext_;
};

#endif  // __Pipeline