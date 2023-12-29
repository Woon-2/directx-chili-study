#ifndef __Graphics
#define __Graphics

#include "App/ChiliWindow.hpp"
#include "GFXFactory.hpp"
#include "Pipeline.hpp"
#include "GFXSwapChain.hpp"
#include "GraphicsStorage.hpp"

class Graphics {
public:
    using MyWindow = ChiliWindow;

    Graphics(MyWindow& wnd);

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    void present();
    void clear(float r, float g, float b);

    GFXFactory factory() noexcept {
        return factory_;
    }

    GFXPipeline pipeline() noexcept {
        return pipeline_;
    }

private:
    void initGFXComponents();
    void constructAppRenderTarget();

    DXGI_SWAP_CHAIN_DESC GFXComponentsDesc() const noexcept {
        return DXGI_SWAP_CHAIN_DESC{
            .BufferDesc = DXGI_MODE_DESC{
                .Width = 0,
                .Height = 0,
                .RefreshRate = DXGI_RATIONAL{
                    .Numerator = 0,
                    .Denominator = 0
                },
                .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
                .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
                .Scaling = DXGI_MODE_SCALING_UNSPECIFIED
            },
            .SampleDesc = DXGI_SAMPLE_DESC {
                .Count = 1,
                .Quality = 0
            },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 2,
            .OutputWindow = wnd_.nativeHandle(),
            .Windowed = true,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .Flags = 0
        };
    }

    MyWindow& wnd_;
    GFXStorage storage_;
    GFXFactory factory_;
    GFXPipeline pipeline_;
    GFXSwapChain swapchain_;

    GFXStorage::ID IDAppRenderTarget_;
};

#endif  // __Graphics