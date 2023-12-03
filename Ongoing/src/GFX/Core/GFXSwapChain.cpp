#include "GFX/Core/GFXSwapChain.hpp"

#include "GFX/Core/GraphicsException.hpp"

void GFXSwapChain::present() {
    if ( auto hr = pSwapChain_->Present(2u, 0u); FAILED(hr) ) {
        if (hr == DXGI_ERROR_DEVICE_REMOVED) {
            setDeviceRemoved(true);
        }
        else {
            throw GFX_EXCEPT(hr);
        }
    }  
}