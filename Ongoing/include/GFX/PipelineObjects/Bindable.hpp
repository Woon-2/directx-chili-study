#ifndef __Bindable
#define __Bindable

#define ACTIVATE_BINDABLE_LOG

#include "Game/GFXCMDLogger.hpp"

#include <utility>

class GFXPipeline;

class IBindable {
public:
    friend class GFXPipeline;

#ifdef ACTIVATE_BINDABLE_LOG
protected:
    class LogComponent {
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

private:
    virtual void bind(GFXPipeline& pipeline) = 0;
};

template <class T>
class BinderInterface {
public:
    BinderInterface()
        : bLocalRebindEnabled_(false) {}

    bool localRebindEnabled() const noexcept {
        bLocalRebindEnabled_;
    }

    static bool globalRebindEnabled() noexcept {
        return bGlobalRebindEnabled;
    }

    void enableLocalRebind() noexcept {
        bLocalRebindEnabled_ = true;
    }

    static void enableGlobalRebind() noexcept {
        return bGlobalRebindEnabled;
    }

    template <class ... Args>
    [[maybe_unused]] bool bind(Args&& ... args) {
        if (pLastBinder == this) {
            if ( !(bGlobalRebindEnabled || bLocalRebindEnabled_) ) {
                return false;
            }
        }

        pLastBinder = this;

        static_cast<T*>(this)->doBind( std::forward<Args>(args)... );

        return true;
    }

private:
    static const BinderInterface* pLastBinder;
    static bool bGlobalRebindEnabled;
    bool bLocalRebindEnabled_;
};

template <class T>
const BinderInterface<T>* BinderInterface<T>::pLastBinder;

template <class T>
bool BinderInterface<T>::bGlobalRebindEnabled = false;

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

    void disableLocalRebind() noexcept {
        static_cast<T*>(this)->binder_.disableLocalRebind();
    }

private:
};

#endif  // __Bindable