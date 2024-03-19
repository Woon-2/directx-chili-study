#include "GFX/Core/SwapChain.hpp"

#include "GFX/Core/Exception.hpp"

namespace gfx {

void GFXSwapChain::present() {
    if ( auto hr = pSwapChain_->Present(1u, 0u); FAILED(hr) ) {
        if (hr == DXGI_ERROR_DEVICE_REMOVED) {
            setDeviceRemoved(true);
        }
        else {
            throw GFX_EXCEPT(hr);
        }
    }  
}

}   // namespace gfx