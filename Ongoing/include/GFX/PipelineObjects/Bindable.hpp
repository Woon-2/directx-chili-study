#ifndef __Bindable
#define __Bindable

#define ACTIVATE_BINDABLE_LOG

#include "Game/GFXCMDLogger.hpp"

#include <utility>
#include <array>
#include <ranges>
#include <algorithm>

class GFXPipeline;

class IBindable {
public:
    friend class GFXPipeline;

    virtual ~IBindable() = 0 {}

#ifdef ACTIVATE_BINDABLE_LOG
protected:
    class LogComponent;
    // TODO: add SlotLogComponent
#endif  // ACTIVATE_BINDABLE_LOG

private:
    virtual void bind(GFXPipeline& pipeline) = 0;
};

#ifdef ACTIVATE_BINDABLE_LOG
class IBindable::LogComponent {
public:
    LogComponent() noexcept
        : category_(), logSrc_(nullptr), bLogEnabled_(false) {}
    
    LogComponent( const void* parent,
        const GFXCMDSourceCategory& category
    ) noexcept
        : category_(category), logSrc_(parent), bLogEnabled_(false) {}

    LogComponent( const void* parent,
        GFXCMDSourceCategory&& category
    ) noexcept
        : category_( std::move(category) ),
        logSrc_(parent), bLogEnabled_(false) {}

    void enableLog() noexcept {
        bLogEnabled_ = true;
    }

    void disableLog() noexcept {
        bLogEnabled_ = false;
    }

    bool logEnabled() const noexcept {
        return bLogEnabled_;
    }

    void logCreate() const {
        logImpl(GFXCMDType::Create);
    }

    void logBind() const {
        logImpl(GFXCMDType::Bind);
    }

private:
    void logImpl(GFXCMDType cmdType) const {
        GFXCMDLOG.logCMD( GFXCMDDesc{
            .cmdType = cmdType,
            .sources = { GFXCMDSource{
                .category = category_,
                .pSource = logSrc_
            } }
        } );
    }

    GFXCMDSourceCategory category_;
    const void* logSrc_;
    bool bLogEnabled_;
};
#endif  // ACTIVATE_BINDABLE_LOG

template <class T>
class BinderInterface {
public:
    BinderInterface()
        : bLocalRebindEnabled_(false),
        bLocalRebindTemporary_(false) {}

    bool localRebindEnabled() const noexcept {
        return bLocalRebindEnabled_ || bLocalRebindTemporary_;
    }

    static bool globalRebindEnabled() noexcept {
        return bGlobalRebindEnabled || bGlobalRebindTemporary;
    }

    void enableLocalRebind() noexcept {
        bLocalRebindEnabled_ = true;
    }

    void enableLocalRebindTemporary() noexcept {
        bLocalRebindTemporary_ = true;
    }

    static void enableGlobalRebind() noexcept {
        bGlobalRebindEnabled = true;
    }

    static void enableGlobalRebindTemporary() noexcept {
        bGlobalRebindTemporary = true;
    }

    void disableLocalRebind() noexcept {
        bLocalRebindEnabled_ = false;
        bLocalRebindTemporary_ = false;
    }

    static void disableGlobalRebind() noexcept {
        bGlobalRebindEnabled = false;
        bGlobalRebindTemporary = false;
    }

    template <class ... Args>
    [[maybe_unused]] bool bind(Args&& ... args) {
        if (pLastBinder == this) {
            if ( !(localRebindEnabled() || globalRebindEnabled()) ) {
                return false;
            }
        }

        pLastBinder = this;

        static_cast<T*>(this)->doBind( std::forward<Args>(args)... );

        bGlobalRebindTemporary = false;
        bLocalRebindTemporary_ = false;

        return true;
    }

private:
    static const BinderInterface* pLastBinder;
    static bool bGlobalRebindEnabled;
    static bool bGlobalRebindTemporary;
    bool bLocalRebindEnabled_;
    bool bLocalRebindTemporary_;
};

template <class T>
const BinderInterface<T>* BinderInterface<T>::pLastBinder;

template <class T>
bool BinderInterface<T>::bGlobalRebindEnabled = false;

template <class T>
bool BinderInterface<T>::bGlobalRebindTemporary = false;

namespace detail {
// if shader utilizes slots over than gMaximumSlots,
// update gMaximumSlots with higher value,
// or SEGFAULT will take place.
inline constexpr std::size_t gMaximumSlots = 32u;
}

template <class T>
class SlotBinderInterface {
public:
    SlotBinderInterface()
        : bLocalRebindEnabled_{false},
        bLocalRebindTemporary_{false} {}

    bool localRebindEnabled(std::size_t slot) const noexcept {
        return bLocalRebindEnabled_[slot] || bLocalRebindTemporary_[slot];
    }

    void enableLocalRebind(std::size_t slot) {
        bLocalRebindEnabled_[slot] = true;
    }

    void enableLocalRebindTemporary(std::size_t slot) {
        bLocalRebindTemporary_[slot] = true;
    }

    void disableLocalRebind(std::size_t slot) {
        bLocalRebindEnabled_[slot] = false;
        bLocalRebindTemporary_[slot] = false;
    }

    static bool globalRebindEnabled(std::size_t slot) {
        return bGlobalRebindEnabled[slot] || bGlobalRebindTemporary[slot];
    }

    static void enableGlobalRebind(std::size_t slot) {
        bGlobalRebindEnabled[slot] = true;
    }

    static void enableGlobalRebindTemporary(std::size_t slot) {
        bGlobalRebindTemporary[slot] = true;
    }

    static void disableGlobalRebind(std::size_t slot) {
        bGlobalRebindEnabled[slot] = false;
        bGlobalRebindTemporary[slot] = false;
    }

    template <class ... Args>
    [[maybe_unused]] bool bind(std::size_t slot, Args&& ... args) {
        if ( pLastBinder[slot] == this &&
            !(localRebindEnabled(slot) || globalRebindEnabled(slot))
        ) {
            return false;
        }

        pLastBinder[slot] = this;

        static_cast<T*>(this)->doBind( std::forward<Args>(args)... );
        
        bGlobalRebindTemporary[slot] = false;
        bLocalRebindTemporary_[slot] = false;

        return true;
    }

private:
    static std::array< const SlotBinderInterface<T>*,
        detail::gMaximumSlots
    > pLastBinder;
    static std::array<bool, detail::gMaximumSlots> bGlobalRebindEnabled;
    static std::array<bool, detail::gMaximumSlots> bGlobalRebindTemporary;
    std::array<bool, detail::gMaximumSlots> bLocalRebindEnabled_;
    std::array<bool, detail::gMaximumSlots> bLocalRebindTemporary_;
};

template <class T>
std::array< const SlotBinderInterface<T>*, detail::gMaximumSlots >
SlotBinderInterface<T>::pLastBinder { nullptr };

template <class T>
std::array<bool, detail::gMaximumSlots>
SlotBinderInterface<T>::bGlobalRebindEnabled { false };

template <class T>
std::array<bool, detail::gMaximumSlots>
SlotBinderInterface<T>::bGlobalRebindTemporary { false };

template <class T>
class LocalRebindInterface {
public:
    bool localRebindEnabled() const noexcept {
        return static_cast<const T*>(this)
            ->binder_.localRebindEnabled();
    }

    void enableLocalRebind() noexcept {
        static_cast<T*>(this)->binder_.enableLocalRebind();
    }

    void enableLocalRebindTemporary() noexcept {
        static_cast<T*>(this)->binder_.enableLocalRebindTemporary();
    }

    void disableLocalRebind() noexcept {
        static_cast<T*>(this)->binder_.disableLocalRebind();
    }

private:
};

template <class T>
class SlotLocalRebindInterface {
public:
    bool localRebindEnabled(std::size_t slot) const noexcept {
        return static_cast<const T*>(this)
            ->binder_.localRebindEnabled(slot);
    }

    void enableLocalRebind(std::size_t slot) noexcept {
        static_cast<T*>(this)->binder_.enableLocalRebind(slot);
    }

    void enableLocalRebindTemporary(std::size_t slot) noexcept {
        static_cast<T*>(this)->binder_.enableLocalRebindTemporary(slot);
    }

    void disableLocalRebind(std::size_t slot) noexcept {
        static_cast<T*>(this)->binder_.disableLocalRebind(slot);
    }
private:
};

#endif  // __Bindable