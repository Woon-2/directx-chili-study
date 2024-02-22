#include "GFX/Core/Exception.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>
#include <string>
#include <sstream>

#ifndef NDEBUG
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

DXGIInfoMsgContainer<std::string> BasicDXGIDebugLogger::peekMessages(
    SeverityFlags severityFlags
) const {
    auto f = ScopedSeverityFilter( pDXGIInfoQueue_.Get(), severityFlags );

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

DXGIInfoMsgContainer<std::string> BasicDXGIDebugLogger::getMessages(
    SeverityFlags severityFlags
) {
    auto ret = peekMessages(severityFlags);
    clear();
    return ret;
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

BasicDXGIDebugLogger::BasicDXGIDebugLogger()
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

BasicDXGIDebugLogger& getLogger() {
    static std::optional<BasicDXGIDebugLogger> inst;

    if (!inst.has_value()) {
        inst = BasicDXGIDebugLogger();
    }

    return inst.value();
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