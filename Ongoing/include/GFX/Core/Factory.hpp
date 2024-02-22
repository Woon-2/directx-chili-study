#ifndef __GFXFactory
#define __GFXFactory

#include <d3d11.h>
#include "Namespaces.hpp"

class GFXFactory {
public:
    GFXFactory() = default;

    GFXFactory(wrl::ComPtr<ID3D11Device> pDevice)
        : pDevice_(pDevice) {}

    void setDevice(wrl::ComPtr<ID3D11Device> pDevice) noexcept {
        pDevice_ = pDevice;
    }

    wrl::ComPtr<ID3D11Device> device() const noexcept {
        return pDevice_;
    }

    decltype(auto) operator&() {
        return &pDevice_;
    }

    decltype(auto) operator&() const {
        return &pDevice_;
    }

    GFXFactory* address() noexcept {
        return this;
    }

    const GFXFactory* address() const noexcept {
        return this;
    }

private:
    wrl::ComPtr<ID3D11Device> pDevice_;
};


#endif  // __GFXFactory