#ifndef __Graphics
#define __Graphics

#include "ChiliWindow.hpp"
#include <d3d11.h>

#ifndef NDEBUG
#include <dxgidebug.h>
#endif  // NDEBUG

#include <vector>
#include <string>
#include <cstddef>

#include "Woon2Exception.hpp"

#define GFX_EXCEPT_NOINFO(hr) GraphicsException(__LINE__, __FILE__, (hr))
#define GFX_THROW_FAILED_NOINFO(hrcall) \
    if ( HRESULT hr = (hrcall); hr < 0 ) {  \
        throw GFX_EXCEPT_NOINFO(hr);    \
    }

#ifdef NDEBUG
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO((hr))
#define GFX_THROW_FAILED(hrcall) GFX_THROW_FAILED_NOINFO((hrcall))
#else
#define GFX_DEVICE_REMOVED_EXCEPT(hr) \
    DeviceRemovedException(__LINE__, __FILE__, (hr), getLogger().getMessages())
#define GFX_EXCEPT(hr) GraphicsException(__LINE__, __FILE__, (hr), getLogger().getMessages())
#define GFX_THROW_FAILED(hrcall) \
    if ( HRESULT hr = (hrcall); hr < 0 ) {  \
        throw GFX_EXCEPT(hr);   \
    }
#define GFX_CLEAR_LOG() getLogger().clear()
#endif  // NDEBUG

void releaseCOM(IUnknown* com);

#ifndef NDEBUG
template <class T>
using DXGIInfoMsgContainer = std::vector<T>;

class BasicDXGIDebugLogger {
public:
    BasicDXGIDebugLogger(const BasicDXGIDebugLogger&) = default;
    BasicDXGIDebugLogger& operator=(const BasicDXGIDebugLogger&) = default;
    BasicDXGIDebugLogger(BasicDXGIDebugLogger&&) noexcept = default;
    BasicDXGIDebugLogger& operator=(BasicDXGIDebugLogger&&) noexcept = default;
    ~BasicDXGIDebugLogger() {
        releaseCOM(pDXGIInfoQueue_);
    }

    void clear() noexcept {
        pDXGIInfoQueue_->ClearStoredMessages(DXGI_DEBUG_ALL);
    }

    DXGIInfoMsgContainer<std::string> getMessages() const {
        auto ret = DXGIInfoMsgContainer<std::string>();
        auto nMsgs = pDXGIInfoQueue_->GetNumStoredMessages(DXGI_DEBUG_ALL);
        ret.reserve(nMsgs);

        for (auto i = decltype(nMsgs)(0); i < nMsgs; ++i) {
            auto msgLength = size_t(0ull);
            WND_THROW_FAILED( pDXGIInfoQueue_->GetMessage(
                DXGI_DEBUG_ALL, i, nullptr, &msgLength
            ));

            auto bytes = std::vector<std::byte>(msgLength);

            auto pMsg = reinterpret_cast<
                DXGI_INFO_QUEUE_MESSAGE*
            >(bytes.data());

            WND_THROW_FAILED( pDXGIInfoQueue_->GetMessage(
                DXGI_DEBUG_ALL, i, pMsg, &msgLength
            ));

            ret.emplace_back( pMsg->pDescription );
        }

        return ret;
    }

    friend BasicDXGIDebugLogger& getLogger();

private:
    BasicDXGIDebugLogger()
        : pDXGIInfoQueue_(nullptr) {
        using fPtr = HRESULT(__stdcall *)(REFIID, void**);

        const auto hModDXGIDebug = LoadLibraryExA(
            "dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32
        );

        fPtr pDXGIGetDebugInterface = reinterpret_cast<fPtr>(
            reinterpret_cast<void*>(
                GetProcAddress(hModDXGIDebug, "DXGIGetDebugInterface")
            )
        );

        if (!pDXGIGetDebugInterface) {
            throw WND_LAST_EXCEPT();
        }

        (*pDXGIGetDebugInterface)(__uuidof(IDXGIInfoQueue),
            reinterpret_cast<void**>(&pDXGIInfoQueue_)
        );
    }

    IDXGIInfoQueue* pDXGIInfoQueue_;
};

BasicDXGIDebugLogger& getLogger();

#endif  // NDEBUG

#ifdef NDEBUG
// Graphics Exception for Release Mode
class GraphicsException : public Win32::WindowException {
public:
    using Win32::WindowException::WindowException;
    const char* type() const noexcept override;

private:
};
#else
// Graphics Exception for Debug Mode
class GraphicsException : public Win32::WindowException {
public:
    GraphicsException(int line, const char* file, HRESULT hr,
        const DXGIInfoMsgContainer<std::string>& msgs = {});

    const char* what() const noexcept override;
    const char* type() const noexcept override;
    bool emptyInfo() const noexcept { return info_.empty(); }
    const std::string& info() const noexcept { return info_; }

private:
    std::string info_;
};
#endif  // NDEBUG

class DeviceRemovedException : public GraphicsException {
public:
    using GraphicsException::GraphicsException;
    const char* type() const noexcept override;

private:
};

template <class Wnd>
class Graphics {
public:
    using MyWindow = Wnd;

    Graphics(MyWindow& wnd)
        : wnd_(wnd), pDevice_(nullptr), pSwap_(nullptr),
        pContext_(nullptr), pTarget_(nullptr) {
        try {

        auto sd = DXGI_SWAP_CHAIN_DESC{
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
            .BufferCount = 1,
            .OutputWindow = HWND(696969),
            .Windowed = true,
            .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
            .Flags = 0
        };

        UINT swapCreateFlags = 0u;
        #ifndef NDEBUG
        swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif

        // create device and front/back buffers, swap chain, and rendering context
        GFX_THROW_FAILED( D3D11CreateDeviceAndSwapChain(
            /* pAdapter = */ nullptr,
            /* DriverType = */ D3D_DRIVER_TYPE_HARDWARE,
            /* Software = */ nullptr,
            /* Flags = */ swapCreateFlags,
            /* pFeatureLevels = */ nullptr,
            /* FeatureLevels = */ 0,
            /* SDKVersion = */ D3D11_SDK_VERSION,
            /* pSwapChainDesc = */ &sd,
            /* ppSwapChain = */ &pSwap_,
            /* ppDevice = */ &pDevice_,
            /* pFeatureLevel = */ nullptr,
            /* ppImmediateContext = */ &pContext_
        ) );

        // TODO: must use RAII
        ID3D11Resource* pBackBuffer = nullptr;
        GFX_THROW_FAILED(
            pSwap_->GetBuffer( 0, __uuidof(ID3D11Resource),
                reinterpret_cast<void**>(&pBackBuffer)
            )
        );
        GFX_THROW_FAILED(
            pDevice_->CreateRenderTargetView(
                pBackBuffer,
                nullptr,
                &pTarget_
            )
        );
        pBackBuffer->Release();

        }
        catch (const Win32::WindowException& e) {
            MessageBoxA(nullptr, e.what(), "Window Exception",
                MB_OK | MB_ICONEXCLAMATION);
        }
        catch (const std::exception& e) {
            MessageBoxA(nullptr, e.what(), "Standard Exception",
                MB_OK | MB_ICONEXCLAMATION);
        }
        catch(...) {
            MessageBoxA(nullptr, "no details available",
                "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
        }
    }

    ~Graphics() {
        releaseCOM(pDevice_);
        releaseCOM(pSwap_);
        releaseCOM(pContext_);
        releaseCOM(pTarget_);
    }

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    void render() {
        if ( auto hr = pSwap_->Present(2u, 0u); hr < 0 ) {
            if (hr == DXGI_ERROR_DEVICE_REMOVED) {
                throw GFX_DEVICE_REMOVED_EXCEPT(hr);
            }
            else {
                throw GFX_EXCEPT(hr);
            }
        }
    }

    void clear( float r, float g, float b ) {
        const float color[] = { r, g, b, 1.f };
        pContext_->ClearRenderTargetView( pTarget_, color );
    }

private:
    static void releaseCOM(IUnknown* com) {
        if (com) {
            com->Release();
            com = nullptr;
        }
    }

    MyWindow& wnd_;
    ID3D11Device* pDevice_;
    IDXGISwapChain* pSwap_;
    ID3D11DeviceContext* pContext_;
    ID3D11RenderTargetView* pTarget_;
};

#endif  // __Graphics