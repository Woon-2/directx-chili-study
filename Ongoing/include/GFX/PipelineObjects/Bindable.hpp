#ifndef __Bindable
#define __Bindable

#include <utility>

class GFXPipeline;

class IBindable {
public:
    friend class GFXPipeline;

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
    void bind(Args&& ... args) {
        if (pLastBinder == this) {
            if ( !(bGlobalRebindEnabled || bLocalRebindEnabled_) ) {
                return;
            }
        }

        pLastBinder = this;

        static_cast<T*>(this)->doBind( std::forward<Args>(args)... );
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