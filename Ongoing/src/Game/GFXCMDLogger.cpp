#include "Game/GFXCMDLogger.hpp"

#include <optional>

void GFXCMDLogger::CountLogger::advance() {
    idx_ = (idx_ + 1) % historySize;
    size_ = std::min(size_ + 1, historySize);

    // if clear request from previous frame exist,
    // clear current history slot.
    handleClearRequest();

    // if history is full, empty oldest slot and use it.
    if (size() == historySize) [[unlikely]] {
        requestClear(idx_);
    }
}

void GFXCMDLogger::CountLogger::log(const void* val) {
    // if clear request from current or previous frame exist,
    // clear current history slot.
    handleClearRequest();
    history_[idx_][val] += 1u;
}

template <class Rep>
Rep GFXCMDLogger::CountLogger::count(const void* pSource, std::size_t nFrame) const {
    auto sum = Count(0);
    nFrame = std::min(size(), nFrame);

    // regular algorithm
    if ( (idx_ - nFrame) <= idx_ ) {
        for (std::size_t i = idx_ - nFrame; i < idx_; ++i) {
            if ( history_[i].contains(pSource) ) {
                sum += history_[i].at(pSource);
            }
        }
    }
    // if a wrap around occured
    else {
        // get idx_ elements from front
        for (std::size_t i = 0u; i < idx_; ++i) {
            if ( history_[i].contains(pSource) ) {
                sum += history_[i].at(pSource);
            }
        }
        // and get nFrame - idx_ elements from back
        for (std::size_t i = historySize - (nFrame - idx_);
            i < historySize; ++i
        ) {
            if ( history_[i].contains(pSource) ) {
                sum += history_[i].at(pSource);
            }
        }
    }

    return static_cast<Rep>(sum);
}

template <class Rep>
Rep GFXCMDLogger::CountLogger::averageCount(const void* pSource, std::size_t nFrame) const {
    auto average = FloatCount(0);
    nFrame = std::min(size(), nFrame);
    auto denom = static_cast<FloatCount>(nFrame);

    // regular algorithm
    if ( (idx_ - nFrame) <= idx_ ) {
        for (std::size_t i = idx_ - nFrame; i < idx_; ++i) {
            if ( history_[i].contains(pSource) ) {
                average += history_[i].at(pSource) / denom;
            }
        }
    }
    // if a wrap around occured
    else {
        // get idx_ elements from front
        for (std::size_t i = 0u; i < idx_; ++i) {
            if ( history_[i].contains(pSource) ) {
                average += history_[i].at(pSource) / denom;
            }
        }
        // and get nFrame - idx_ elements from back
        for (std::size_t i = historySize - (nFrame - idx_);
            i < historySize; ++i
        ) {
            if ( history_[i].contains(pSource) ) {
                average += history_[i].at(pSource) / denom;
            }
        }
    }

    return static_cast<Rep>(average);
}

void GFXCMDLogger::History::log(GFXCMDType cmdType, const void* pSource) {
    switch (cmdType) {
    case GFXCMDType::Create:
        logCreate_.log(pSource);
        break;

    case GFXCMDType::Bind:
        logBind_.log(pSource);
        break;

    case GFXCMDType::Draw:
        logDraw_.log(pSource);
        break;
    }
}

template <class Rep>
Rep GFXCMDLogger::History::count( GFXCMDFilter cmdFilter,
    const void* pSource, std::size_t nFrame
) const {
    auto ret = Rep(0);

    if (cmdFilter & GFXCMDType::Create) {
        ret += logCreate_.count<Rep>(pSource, nFrame);
    }

    if (cmdFilter & GFXCMDType::Bind) {
        ret += logBind_.count<Rep>(pSource, nFrame);
    }

    if (cmdFilter & GFXCMDType::Draw) {
        ret += logDraw_.count<Rep>(pSource, nFrame);
    }

    return ret;
}

template <class Rep>
Rep GFXCMDLogger::History::averageCount(
    GFXCMDFilter cmdFilter, const void* pSource,
    std::size_t nFrame, bool preventOverflow
) const {
    auto ret = Rep(0);

    if (empty()) [[unlikely]] {
        // protect from division by zero
        return ret;
    }

    auto accumulate = [&ret, preventOverflow](
        auto& countLogger, const void* pSource, std::size_t nFrame
    ) {
        if (preventOverflow) {
            ret += countLogger.averageCount<Rep>(pSource, nFrame);
        }
        else {
            ret += countLogger.count<Rep>(pSource, nFrame)
                / std::min( nFrame, countLogger.size() );
        }
    };

    if (cmdFilter & GFXCMDType::Create) {
        accumulate(logCreate_, pSource, nFrame);
    }

    if (cmdFilter & GFXCMDType::Bind) {
        accumulate(logBind_, pSource, nFrame);
    }

    if (cmdFilter & GFXCMDType::Draw) {
        accumulate(logDraw_, pSource, nFrame);
    }

    return ret;
}

GFXCMDLogger::GFXCMDLogger()
    : storage_() {
    storage_.try_emplace( GFXCMDSourceCategory("total"), History() );
}

void GFXCMDLogger::addCategory(const GFXCMDSourceCategory& category) {
    auto [_, added] = storage_.try_emplace( category, History() );

    if (!added) {
        // ignore or warn or error
    }
}

void GFXCMDLogger::logCMD(const GFXCMDDesc& desc) {
    // log for received sources.
    std::ranges::for_each( desc.sources,
        [this, cmdType = desc.cmdType](auto&& source) {
            checkCategory(source.category);
            storage_[source.category].log(cmdType, source.pSource);
        }
    );

    // additionally log for current entry stack.
    std::ranges::for_each( entryStack_,
        [this, cmdType = desc.cmdType](auto&& source) {
            checkCategory(source.category);
            storage_[source.category].log(cmdType, source.pSource);
        }
    );

    storage_.at( GFXCMDSourceCategory("total") ).log(
        desc.cmdType, sourceTotal
    );
}

void GFXCMDLogger::advance() {
    std::ranges::for_each( storage_, [](auto& pair) {
        pair.second.advance();
    } );
}

GFXCMDLogger::Count GFXCMDLogger::CMDCnt( GFXCMDFilter cmdFilter,
    const GFXCMDSource& cmdSrc, std::size_t nFrame
) const {
    checkCategory(cmdSrc.category);
    return storage_[cmdSrc.category].count<Count>(
        cmdFilter, cmdSrc.pSource, nFrame
    );
}

GFXCMDLogger::FloatCount GFXCMDLogger::CMDCntF( GFXCMDFilter cmdFilter,
    const GFXCMDSource& cmdSrc, std::size_t nFrame
) const {
    checkCategory(cmdSrc.category);
    return storage_[cmdSrc.category].count<FloatCount>(
        cmdFilter, cmdSrc.pSource, nFrame
    );
}

GFXCMDLogger::Count GFXCMDLogger::avCMDCnt( GFXCMDFilter cmdFilter,
    const GFXCMDSource& cmdSrc, std::size_t nFrame
) const {
    checkCategory(cmdSrc.category);
    return storage_[cmdSrc.category].averageCount<Count>(
        cmdFilter, cmdSrc.pSource, nFrame
    );
}

GFXCMDLogger::FloatCount GFXCMDLogger::avCMDCntF( GFXCMDFilter cmdFilter,
    const GFXCMDSource& cmdSrc, std::size_t nFrame
) const {
    checkCategory(cmdSrc.category);
    return storage_[cmdSrc.category].averageCount<FloatCount>(
        cmdFilter, cmdSrc.pSource, nFrame
    );
}

GFXCMDLogger& getGFXCMDLogger() {
    static auto inst = std::optional<GFXCMDLogger>();

    if (!inst.has_value()) {
        inst = GFXCMDLogger();
    }

    return inst.value();
}