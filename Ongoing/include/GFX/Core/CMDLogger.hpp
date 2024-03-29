#ifndef __GFXCMDLogger
#define __GFXCMDLogger

#include <array>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <ranges>
#include <type_traits>

#include <string>
#include <string_view>
#include <utility>

#include "Literal.hpp"
#include "EnumUtil.hpp"
#include "OneHotEncode.hpp"

// must lock logger for future multi-thread support,
// or date race will take place.

#define GFXCMDLOG gfx::getGFXCMDLogger()

namespace gfx {

class GFXCMDSourceCategory {
public:
    using MyChar = char;
    using MyString = std::basic_string<MyChar>;
    using MyStringView = std::basic_string_view<MyChar>;

    struct Hash {
        std::size_t operator()(const GFXCMDSourceCategory& key) const noexcept {
            return std::hash<MyString>{}(key.data_);
        }
    };

    constexpr GFXCMDSourceCategory() = default;

    constexpr GFXCMDSourceCategory(const MyChar* data)
        : data_(data) {}

    constexpr GFXCMDSourceCategory(const MyChar* data, std::size_t n)
        : data_(data, n) {}

    constexpr GFXCMDSourceCategory(const MyString& data)
        : data_(data) {}

    constexpr GFXCMDSourceCategory(MyString&& data)
        : data_( std::move(data) ) {}

    constexpr GFXCMDSourceCategory(MyStringView data)
        : data_(data) {}

    constexpr MyStringView value() const noexcept {
        return data_;
    }

    friend constexpr auto operator<=>( const GFXCMDSourceCategory& lhs,
        const GFXCMDSourceCategory& rhs
    ) noexcept = default;

private:
    MyString data_;
};

struct GFXCMDSource {
    GFXCMDSourceCategory category;
    const void* pSource;
};

enum class GFXCMDType {
    ONEHOT_ENCODE(Create, Bind, Draw)
};

DEFINE_ENUM_LOGICAL_OP_ALL(GFXCMDType)
using GFXCMDFilter = std::underlying_type_t<GFXCMDType>;

struct GFXCMDDesc {
    GFXCMDType cmdType;
    std::vector<GFXCMDSource> sources;
};

class GFXCMDLogger {
public:
    static constexpr std::size_t historySize = 160u;
    using Count = std::size_t;
    using FloatCount = double;

private:
    static constexpr auto sourceTotal = nullptr;

    class CountLogger {
    public:
        CountLogger()
            : history_{}, idx_(0), size_(0),
            idxClear_(0), bClear_(false) {}

        void swap(CountLogger& other) {
            std::swap(history_, other.history_);
            std::swap(idx_, other.idx_);
        }

        void advance();
        void log(const void* val);

        template <class Rep>
        Rep count(const void* pSource, std::size_t nFrame) const;
        template <class Rep>
        Rep averageCount(const void* pSource, std::size_t nFrame) const;

        std::size_t size() const noexcept {
            return size_;
        }

    private:
        void requestClear(std::size_t idxClear) {
            idxClear_ = idxClear;
            bClear_ = true;
        }

        void handleClearRequest() {
            if (bClear_) [[unlikely]] {
                history_[idxClear_].clear();
                bClear_ = false;
            }
        }

        std::array<
            std::unordered_map<const void*, Count>,
            historySize
        > history_;
        std::size_t idx_;
        std::size_t size_;

        std::size_t idxClear_;
        bool bClear_;
    };

    class History {
    public:
        History()
            : logCreate_(), logBind_(), logDraw_() {}

        void log(GFXCMDType cmdType, const void* pSource);

        template <class Rep>
        Rep count( GFXCMDFilter cmdFilter,
            const void* pSource, std::size_t nFrame
        ) const;

        template <class Rep>
        Rep countTotal(const void* pSource, std::size_t nFrame) const {
            return count(GFXCMDType::Create | GFXCMDType::Bind | GFXCMDType::Draw);
        }

        template <class Rep>
        Rep averageCount( GFXCMDFilter cmdFilter,
            const void* pSource, std::size_t nFrame,
            bool preventOverflow = false
        ) const;

        template <class Rep>
        Rep averageCountTotal(const void* pSource, std::size_t nFrame) const {
            return averageCount(GFXCMDType::Create | GFXCMDType::Bind | GFXCMDType::Draw);
        }

        void advance() {
            logCreate_.advance();
            logBind_.advance();
            logDraw_.advance();
        }

        std::size_t size() const noexcept {
            // all logger share same size.
            return logCreate_.size();
        }

        bool empty() const noexcept {
            return !size();
        }

    private:
        CountLogger logCreate_;
        CountLogger logBind_;
        CountLogger logDraw_;
        std::size_t size_;
    };

public:
    GFXCMDLogger();

    void addCategory(const GFXCMDSourceCategory& category);
    void addCategory(GFXCMDSourceCategory&& category);
    void removeCategory(const GFXCMDSourceCategory& category) {
        storage_.erase(category);
    }
    void logCMD(const GFXCMDDesc& desc);
    void advance();

    void entryStackPush(const GFXCMDSource& cmdSrc) {
        entryStack_.push_back(cmdSrc);
    }

    const GFXCMDSource& entryStackPeek(
        const GFXCMDSource& cmdSrc
    ) const noexcept {
        return entryStack_.back();
    }

    [[maybe_unused]] const GFXCMDSource entryStackPop() noexcept {
        auto ret = std::move(entryStack_.back());
        entryStack_.pop_back();
        return ret;
    }

    Count CMDCnt(GFXCMDFilter cmdFilter) const {
        return CMDCnt( cmdFilter, GFXCMDSource{
            .category = __LITERAL(GFXCMDSourceCategory::MyChar, "total"),
            .pSource = sourceTotal
        } );
    }

    Count CMDCnt(GFXCMDType cmdType) const {
        // implicitly convert to GFXCMDFilter type
        // to delegate implementation
        return CMDCnt(cmdType | cmdType);
    }

    FloatCount CMDCntF(GFXCMDFilter cmdFilter) const {
        return CMDCntF( cmdFilter, GFXCMDSource{
            .category = __LITERAL(GFXCMDSourceCategory::MyChar, "total"),
            .pSource = sourceTotal
        } );
    }

    FloatCount CMDCntF(GFXCMDType cmdType) const {
        return CMDCntF(cmdType | cmdType);
    }

    Count CMDCnt(GFXCMDFilter cmdFilter, std::size_t nFrame) const {
        return CMDCnt( cmdFilter, GFXCMDSource{
            .category = __LITERAL(GFXCMDSourceCategory::MyChar, "total"),
            .pSource = sourceTotal
        }, nFrame );
    }

    Count CMDCnt(GFXCMDType cmdType, std::size_t nFrame) const {
        return CMDCnt(cmdType | cmdType, nFrame);
    }

    FloatCount CMDCntF(GFXCMDFilter cmdFilter, std::size_t nFrame) const {
        return CMDCntF( cmdFilter, GFXCMDSource{
            .category = __LITERAL(GFXCMDSourceCategory::MyChar, "total"),
            .pSource = sourceTotal
        }, nFrame );
    }

    FloatCount CMDCntF(GFXCMDType cmdType, std::size_t nFrame) const {
        return CMDCntF(cmdType | cmdType, nFrame);
    }

    Count CMDCnt(GFXCMDFilter cmdFilter, const GFXCMDSource& cmdSrc) const {
        return CMDCnt( cmdFilter, cmdSrc, storage_[cmdSrc.category].size() );
    }

    Count CMDCnt(GFXCMDType cmdType, const GFXCMDSource& cmdSrc) const {
        return CMDCnt(cmdType | cmdType, cmdSrc);
    }

    FloatCount CMDCntF(GFXCMDFilter cmdFilter, const GFXCMDSource& cmdSrc) const {
        return CMDCntF( cmdFilter, cmdSrc, storage_[cmdSrc.category].size() );
    }

    FloatCount CMDCntF(GFXCMDType cmdType, const GFXCMDSource& cmdSrc) const {
        return CMDCntF(cmdType | cmdType, cmdSrc);
    }

    Count CMDCnt( GFXCMDFilter cmdFilter,
        const GFXCMDSource& cmdSrc, std::size_t nFrame
    ) const;

    Count CMDCnt( GFXCMDType cmdType,
        const GFXCMDSource& cmdSrc, std::size_t nFrame
    ) const {
        return CMDCnt(cmdType | cmdType, cmdSrc, nFrame);
    }

    FloatCount CMDCntF( GFXCMDFilter cmdFilter,
        const GFXCMDSource& cmdSrc, std::size_t nFrame
    ) const;

    FloatCount CMDCntF( GFXCMDType cmdType,
        const GFXCMDSource& cmdSrc, std::size_t nFrame
    ) const {
        return CMDCntF(cmdType | cmdType, cmdSrc, nFrame);
    }

    Count avCMDCnt(GFXCMDFilter cmdFilter) const {
        return avCMDCnt( cmdFilter, GFXCMDSource{
            .category = __LITERAL(GFXCMDSourceCategory::MyChar, "total"),
            .pSource = sourceTotal
        } );
    }

    Count avCMDCnt(GFXCMDType cmdType) const {
        return avCMDCnt(cmdType | cmdType);
    }

    FloatCount avCMDCntF(GFXCMDFilter cmdFilter) const {
        return avCMDCntF( cmdFilter, GFXCMDSource{
            .category = __LITERAL(GFXCMDSourceCategory::MyChar, "total"),
            .pSource = sourceTotal
        } );
    }

    FloatCount avCMDCntF(GFXCMDType cmdType) const {
        return avCMDCntF(cmdType | cmdType);
    }

    Count avCMDCnt(GFXCMDFilter cmdFilter, std::size_t nFrame) const {
        return avCMDCnt( cmdFilter, GFXCMDSource{
            .category = __LITERAL(GFXCMDSourceCategory::MyChar, "total"),
            .pSource = sourceTotal
        }, nFrame );
    }

    Count avCMDCnt(GFXCMDType cmdType, std::size_t nFrame) const {
        return avCMDCnt(cmdType | cmdType, nFrame);
    }

    FloatCount avCMDCntF(GFXCMDFilter cmdFilter, std::size_t nFrame) const {
        return avCMDCntF( cmdFilter, GFXCMDSource{
            .category = __LITERAL(GFXCMDSourceCategory::MyChar, "total"),
            .pSource = sourceTotal
        }, nFrame );
    }

    FloatCount avCMDCntF(GFXCMDType cmdType, std::size_t nFrame) const {
        return avCMDCntF(cmdType | cmdType, nFrame);
    }

    Count avCMDCnt(GFXCMDFilter cmdFilter, const GFXCMDSource& cmdSrc) const {
        return avCMDCnt( cmdFilter, cmdSrc, storage_[cmdSrc.category].size() );
    }

    Count avCMDCnt(GFXCMDType cmdType, const GFXCMDSource& cmdSrc) const {
        return avCMDCnt(cmdType | cmdType, cmdSrc);
    }

    FloatCount avCMDCntF(GFXCMDFilter cmdFilter, const GFXCMDSource& cmdSrc) const {
        return avCMDCntF( cmdFilter, cmdSrc, storage_[cmdSrc.category].size() );
    }

    FloatCount avCMDCntF(GFXCMDType cmdType, const GFXCMDSource& cmdSrc) const {
        return avCMDCntF(cmdType | cmdType, cmdSrc);
    }

    Count avCMDCnt( GFXCMDFilter cmdFilter,
        const GFXCMDSource& cmdSrc, std::size_t nFrame
    ) const;

    Count avCMDCnt( GFXCMDType cmdType,
        const GFXCMDSource& cmdSrc, std::size_t nFrame
    ) const {
        return avCMDCnt(cmdType | cmdType, cmdSrc, nFrame);
    }

    FloatCount avCMDCntF( GFXCMDFilter cmdFilter,
        const GFXCMDSource& cmdSrc, std::size_t nFrame
    ) const;

    FloatCount avCMDCntF( GFXCMDType cmdType,
        const GFXCMDSource& cmdSrc, std::size_t nFrame
    ) const {
        return avCMDCntF(cmdType | cmdType, cmdSrc, nFrame);
    }

private:
    void checkCategory(GFXCMDSourceCategory category) const {
        if ( !storage_.contains(category) ) {
            // undefined category,
            // ignore or warn or error
        }
    }

    mutable std::unordered_map< GFXCMDSourceCategory,
        History, GFXCMDSourceCategory::Hash
    > storage_;
    std::vector<GFXCMDSource> entryStack_;
};

GFXCMDLogger& getGFXCMDLogger();

}   // namespace gfx

#endif  // __GFXCMDLogger