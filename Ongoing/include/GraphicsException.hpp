#ifndef __GraphicsException
#define __GraphicsException

#include "ChiliWindow.hpp"

#include "GraphicsNamespaces.hpp"
#ifndef NDEBUG
#include <dxgidebug.h>
#endif  // NDEBUG

#include <string>

#define GFX_EXCEPT_NOINFO(hr) GraphicsException(__LINE__, __FILE__, (hr))
#define GFX_THROW_FAILED_NOINFO(hrcall) \
    if ( HRESULT hr = (hrcall); hr < 0 ) {  \
        throw GFX_EXCEPT_NOINFO(hr);    \
    }

#ifdef NDEBUG
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO((hr))
#define GFX_EXCEPT_VOID() GFX_EXCEPT(E_INVALIDARG)
#define GFX_THROW_FAILED(hrcall) GFX_THROW_FAILED_NOINFO((hrcall))
#define GFX_THROW_FAILED_VOID(voidcall) (voidcall)
#else
#define GFX_DEVICE_REMOVED_EXCEPT(hr) \
    DeviceRemovedException(__LINE__, __FILE__, (hr), getLogger().getMessages())
#define GFX_EXCEPT(hr) GraphicsException(__LINE__, __FILE__, (hr), getLogger().getMessages())
#define GFX_EXCEPT_VOID() GFX_EXCEPT(E_INVALIDARG)
#define GFX_THROW_FAILED(hrcall) \
    if ( HRESULT hr = (hrcall); hr < 0 )  \
        throw GFX_EXCEPT(hr)
#define GFX_THROW_FAILED_VOID(voidcall) \
    [&]() {  \
        auto __LoggedMessageSize = getLogger().size();    \
        (voidcall); \
        if ( getLogger().size() != __LoggedMessageSize )   \
            throw GFX_EXCEPT_VOID();    \
    }()
#define GFX_CLEAR_LOG() getLogger().clear()
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
template <class T>
using DXGIInfoMsgContainer = std::vector<T>;

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

#ifndef NDEBUG
class BasicDXGIDebugLogger {
public:
    BasicDXGIDebugLogger(const BasicDXGIDebugLogger&) = default;
    BasicDXGIDebugLogger& operator=(const BasicDXGIDebugLogger&) = default;
    BasicDXGIDebugLogger(BasicDXGIDebugLogger&&) noexcept = default;
    BasicDXGIDebugLogger& operator=(BasicDXGIDebugLogger&&) noexcept = default;
    ~BasicDXGIDebugLogger();

    void clear() noexcept {
        pDXGIInfoQueue_->ClearStoredMessages(DXGI_DEBUG_ALL);
    }

    std::size_t size() const noexcept {
        return pDXGIInfoQueue_->GetNumStoredMessages(DXGI_DEBUG_ALL);
    }

    bool empty() const noexcept {
        return size() == 0u;
    }

    DXGIInfoMsgContainer<std::string> peekMessages() const {
        auto ret = DXGIInfoMsgContainer<std::string>();
        auto nMsgs = pDXGIInfoQueue_->GetNumStoredMessages(DXGI_DEBUG_ALL);
        ret.reserve(nMsgs);

        for (auto i = decltype(nMsgs)(0); i < nMsgs; ++i) {
            auto msgLength = size_t(0ull);
            WND_THROW_FAILED(pDXGIInfoQueue_->GetMessage(
                DXGI_DEBUG_ALL, i, nullptr, &msgLength
            ));

            auto bytes = std::vector<std::byte>(msgLength);

            auto pMsg = reinterpret_cast<
                DXGI_INFO_QUEUE_MESSAGE*
            >(bytes.data());

            WND_THROW_FAILED(pDXGIInfoQueue_->GetMessage(
                DXGI_DEBUG_ALL, i, pMsg, &msgLength
            ));

            ret.emplace_back(pMsg->pDescription, pMsg->DescriptionByteLength);
        }

        return ret;
    }

    DXGIInfoMsgContainer<std::string> getMessages() {
        auto ret = peekMessages();
        clear();
        return ret;
    }

    friend BasicDXGIDebugLogger& getLogger();

private:
    BasicDXGIDebugLogger()
        : pDXGIInfoQueue_(nullptr) {
        using fPtr = HRESULT(CALLBACK*)(REFIID, void**);

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

        WND_THROW_FAILED(
            (*pDXGIGetDebugInterface)(
                __uuidof(IDXGIDebug), &pDXGIDebug_
                )
        );

        WND_THROW_FAILED(
            (*pDXGIGetDebugInterface)(
                __uuidof(IDXGIInfoQueue), &pDXGIInfoQueue_
                )
        );
    }

    wrl::ComPtr<IDXGIInfoQueue> pDXGIInfoQueue_;
    wrl::ComPtr<IDXGIDebug> pDXGIDebug_;
};

BasicDXGIDebugLogger& getLogger();

#endif  // NDEBUG

#endif  // __GraphicsException