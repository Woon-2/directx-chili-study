#ifndef __GraphicsException
#define __GraphicsException

#include "App/ChiliWindow.hpp"

#ifndef NDEBUG
#include <dxgidebug.h>
#endif  // NDEBUG
#include "GraphicsNamespaces.hpp"

#include <string>
#include <bitset>
#include <vector>
#include <ranges>
#include <concepts>
#include <type_traits>

#include "OneHotEncode.hpp"
#include "EnumUtil.hpp"

#define GFX_EXCEPT_NOINFO(hr) GraphicsException(__LINE__, __FILE__, (hr))
#define GFX_THROW_FAILED_NOINFO(hrcall) \
    if ( HRESULT hr = (hrcall); hr < 0 ) {  \
        throw GFX_EXCEPT_NOINFO(hr);    \
    }

#ifdef NDEBUG
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO((hr))
#define GFX_EXCEPT_VOID() GFX_EXCEPT(E_INVALIDARG)
#define GFX_EXCEPT_CUSTOM(description)
#define GFX_THROW_FAILED(hrcall) GFX_THROW_FAILED_NOINFO((hrcall))
#define GFX_THROW_FAILED_VOID(voidcall) (voidcall)
#else
#define GFX_DEVICE_REMOVED_EXCEPT(hr) \
    DeviceRemovedException(__LINE__, __FILE__, (hr), getLogger().getMessages())
#define GFX_EXCEPT(hr) GraphicsException(__LINE__, __FILE__, (hr), getLogger().getMessages())
#define GFX_EXCEPT_VOID() GFX_EXCEPT(E_INVALIDARG)
#define GFX_EXCEPT_CUSTOM(description) GraphicsException(__LINE__, __FILE__, \
        E_INVALIDARG, DXGIInfoMsgContainer<std::string>(1, description) \
    )
#define GFX_THROW_FAILED(hrcall) \
    if ( HRESULT hr = (hrcall); hr < 0 )  \
        throw GFX_EXCEPT(hr)
#define GFX_THROW_FAILED_VOID(voidcall) \
    [&]() {  \
        auto __LoggedMessageSize = getLogger().size(    \
            DXGIDebugLogSeverity::Error | DXGIDebugLogSeverity::Corruption    \
        );    \
        (voidcall); \
        if ( getLogger().size(  \
                DXGIDebugLogSeverity::Error | DXGIDebugLogSeverity::Corruption  \
            ) != __LoggedMessageSize    \
        ) { \
            throw GFX_EXCEPT_VOID();    \
        }   \
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

enum class DXGIDebugLogSeverity {
    ONEHOT_ENCODE(Warning, Error, Corruption)
};

DEFINE_ENUM_LOGICAL_OP_ALL(DXGIDebugLogSeverity)
DEFINE_ENUM_COMPARE_OP_ALL(DXGIDebugLogSeverity)

class BasicDXGIDebugLogger {
public:
    using SeverityFlags = std::bitset<3>;
    using Severity = DXGIDebugLogSeverity;

private:
    class ScopedSeverityFilter {
    public:
        ScopedSeverityFilter( IDXGIInfoQueue* pInfoQueue, SeverityFlags flags )
            : pDXGIInfoQueue_(pInfoQueue) {
            auto allowedSeverities = std::vector<
                DXGI_INFO_QUEUE_MESSAGE_SEVERITY
            >();

            if ( flags.to_ulong() | Severity::Warning ) {
                allowedSeverities.push_back(
                    DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING
                );
            }

            if ( flags.to_ulong() | Severity::Error ) {
                allowedSeverities.push_back(
                    DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR
                );
            }

            if ( flags.to_ulong() | Severity::Corruption ) {
                allowedSeverities.push_back(
                    DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION
                );
            }

            auto filter = DXGI_INFO_QUEUE_FILTER{
                .AllowList = DXGI_INFO_QUEUE_FILTER_DESC{
                    .NumSeverities = static_cast<UINT>( allowedSeverities.size() ),
                    .pSeverityList = allowedSeverities.data()
                },
                .DenyList = DXGI_INFO_QUEUE_FILTER_DESC{}
            };

            pDXGIInfoQueue_->PushRetrievalFilter(
                DXGI_DEBUG_ALL, &filter
            );
        }

        ~ScopedSeverityFilter() {
            pDXGIInfoQueue_->PopRetrievalFilter(DXGI_DEBUG_ALL);
        }

    private:
        IDXGIInfoQueue* pDXGIInfoQueue_;
    };

public:
    BasicDXGIDebugLogger(const BasicDXGIDebugLogger&) = default;
    BasicDXGIDebugLogger& operator=(const BasicDXGIDebugLogger&) = default;
    BasicDXGIDebugLogger(BasicDXGIDebugLogger&&) noexcept = default;
    BasicDXGIDebugLogger& operator=(BasicDXGIDebugLogger&&) noexcept = default;
    ~BasicDXGIDebugLogger();

    void clear() noexcept {
        pDXGIInfoQueue_->ClearStoredMessages(DXGI_DEBUG_ALL);
    }

    std::size_t size(SeverityFlags severityFlags
        = Severity::Warning | Severity::Error | Severity::Corruption
    ) const noexcept {
        auto f = ScopedSeverityFilter( pDXGIInfoQueue_.Get(), severityFlags );
        return pDXGIInfoQueue_->GetNumStoredMessagesAllowedByRetrievalFilters(
            DXGI_DEBUG_ALL
        );
    }

    bool empty(SeverityFlags severityFlags
        = Severity::Warning | Severity::Error | Severity::Corruption
    ) const noexcept {
        return size() == 0u;
    }

    DXGIInfoMsgContainer<std::string> peekMessages( SeverityFlags severityFlags
        = Severity::Warning | Severity::Error | Severity::Corruption
    ) const;
    DXGIInfoMsgContainer<std::string> getMessages( SeverityFlags severityFlags
        = Severity::Warning | Severity::Error | Severity::Corruption
    );

    friend BasicDXGIDebugLogger& getLogger();

private:
    BasicDXGIDebugLogger();

    wrl::ComPtr<IDXGIInfoQueue> pDXGIInfoQueue_;
    wrl::ComPtr<IDXGIDebug> pDXGIDebug_;
};

BasicDXGIDebugLogger& getLogger();
#endif  // NDEBUG

#endif  // __GraphicsException