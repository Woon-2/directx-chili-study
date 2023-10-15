#include "Graphics.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>
#include <string>
#include <sstream>

#ifndef NDEBUG
BasicDXGIDebugLogger& getLogger() {
    static std::optional<BasicDXGIDebugLogger> inst;

    if (!inst.has_value()) {
        inst = BasicDXGIDebugLogger();
    }

    return inst.value();
}

GraphicsException::GraphicsException(int line, const char* file,
    HRESULT hr, const DXGIInfoMsgContainer<std::string>& msgs
) : Win32::WindowException(line, file, hr), info_() {
    // join all messages with newlines into single string
    auto out = std::back_inserter(info_);

    auto insertNewLine = [](const auto& s) {
        return s + '\n';
    };
    std::ranges::copy(msgs
        | std::views::transform(insertNewLine)
        | std::views::join,
        out
    );
}

const char* GraphicsException::what() const noexcept
{
    std::ostringstream oss{};

    oss << type() << '\n';

    oss << "[Error Code] " << errorCode() << '\n'
        << "[Description] " << errorStr() << '\n';

    if (!emptyInfo()) {
        oss << "\n[Error Info] " << info() << '\n';
    }

    oss << this->metaStr() << '\n';

    whatBuffer_ = oss.str();
    return whatBuffer_.c_str();
}

BasicDXGIDebugLogger::~BasicDXGIDebugLogger()
{
    try {
        if (pDXGIDebug_) {
            GFX_THROW_FAILED(pDXGIDebug_->ReportLiveObjects(
                DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL
            ));
        }
    }
    catch (GraphicsException& e) {
        MessageBoxA(nullptr, e.what(), "Graphics Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
}

#endif  // NDEBUG

const char* GraphicsException::type() const noexcept
{
    return "GraphicsException";
}

const char* DeviceRemovedException::type() const noexcept
{
    return "DeviceRemovedException";
}