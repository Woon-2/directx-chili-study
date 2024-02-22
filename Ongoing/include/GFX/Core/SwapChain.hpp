#ifndef __GFXSwapChain
#define __GFXSwapChain

#include <d3d11.h>
#include <dxgi.h>
#include "Namespaces.hpp"

class GFXSwapChain {
public:
    GFXSwapChain()
        : pSwapChain_(), bDeviceRemoved_(false) {}

    GFXSwapChain(wrl::ComPtr<IDXGISwapChain> pSwapChain)
        : pSwapChain_(pSwapChain), bDeviceRemoved_(false) {}

    void setSwapChain(wrl::ComPtr<IDXGISwapChain> pSwapChain) noexcept {
        pSwapChain_ = pSwapChain;
    }

    void present();

    bool deviceRemoved() const noexcept {
        return bDeviceRemoved_;
    }

    void setDeviceRemoved(bool val) {
        bDeviceRemoved_ = val;
    }

    wrl::ComPtr<IDXGISwapChain> swapchain() const noexcept {
        return pSwapChain_;
    }

    decltype(auto) operator&() {
        return &pSwapChain_;
    }

    decltype(auto) operator&() const {
        return &pSwapChain_;
    }

    GFXSwapChain* address() noexcept {
        return this;
    }

    const GFXSwapChain* address() const noexcept {
        return this;
    }

private:
    wrl::ComPtr<IDXGISwapChain> pSwapChain_;
    bool bDeviceRemoved_;
};

#endif  // __GFXSwapChain