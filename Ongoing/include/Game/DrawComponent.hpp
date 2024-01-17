#ifndef __DrawComponent
#define __DrawComponent

#define ACTIVATE_DRAWCOMPONENT_LOG

#include "RenderObjectDesc.hpp"
#include "GFX/Core/GraphicsException.hpp"

#include "GFXCMDLogger.hpp"

#include <optional>
#include <memory>

class BasicDrawCaller;

class IDrawComponent {
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
protected:
    class LogComponent {
    public:
        LogComponent() noexcept
            : logSrc_(nullptr), bLogEnabled_(false) {}
        
        LogComponent( const void* parent,
            bool enableLogOnCreation = true
        ) noexcept
            : logSrc_(parent),
            bLogEnabled_(enableLogOnCreation) {
            // must call corresponding entryStackPop
            // in concrete DrawComponent's constructor.
            entryStackPush();
        }

        LogComponent(const LogComponent& other) noexcept
            : logSrc_(other.logSrc_), bLogEnabled_(other.bLogEnabled_) {
            if (logEnabled()) {
                entryStackPush();
            }
        }

        LogComponent& operator=(LogComponent other) noexcept {
            other.swap(*this);
            return *this;
        }

        LogComponent(LogComponent&& other) noexcept
            : logSrc_(other.logSrc_), bLogEnabled_(other.bLogEnabled_) {
            other.logSrc_ = nullptr;
            other.bLogEnabled_ = false;
        }

        void setLogSrc(const void* src) {
            logSrc_ = src;
        }

        void enableLog() noexcept {
            bLogEnabled_ = true;
        }

        void disableLog() noexcept {
            bLogEnabled_ = false;
        }

        bool logEnabled() const noexcept {
            return bLogEnabled_;
        }

        void entryStackPush() {
            GFXCMDLOG.entryStackPush( GFXCMDSource{
                .category = logCategory(),
                .pSource = logSrc_
            } );
        }

        void entryStackPop() noexcept {
            GFXCMDLOG.entryStackPop();
        }

        void swap(LogComponent& rhs) noexcept {
            std::swap(logSrc_, rhs.logSrc_);
            std::swap(bLogEnabled_, rhs.bLogEnabled_);
        }

    private:
        const void* logSrc_;
        bool bLogEnabled_;
    };
#endif  // ACTIVATE_DRAWCOMPONENT_LOG
public:
    virtual ~IDrawComponent() = 0 {}

    virtual const RenderObjectDesc& renderObjectDesc() const = 0;
    virtual BasicDrawCaller& drawCaller() = 0;
    virtual const BasicDrawCaller& drawCaller() const = 0;

#ifdef ACTIVATE_DRAWCOMPONENT_LOG
    static constexpr const GFXCMDSourceCategory
    logCategory() noexcept {
        return GFXCMDSourceCategory("DrawComponent");
    }
#endif  // ACTIVATE_DRAWCOMPONENT_LOG

    friend auto operator<=>(const IDrawComponent& lhs,
        const IDrawComponent& rhs) {
        return lhs.renderObjectDesc() <=> rhs.renderObjectDesc();
    }
};

class DynDrawCmpBase : public IDrawComponent {
public:
    const RenderObjectDesc& renderObjectDesc() const final override {
        if (!roDesc_.has_value()) {
            throw GFX_EXCEPT_CUSTOM(errMsgNoRODesc());
        }
        return roDesc_.value();
    }

    BasicDrawCaller& drawCaller() final override {
        if (!drawCaller_.has_value()) {
            throw GFX_EXCEPT_CUSTOM(errMsgNoDrawCaller());
        }
        return *drawCaller_.value().get();
    }

    const BasicDrawCaller& drawCaller() const final override {
        if (!drawCaller_.has_value()) {
            throw GFX_EXCEPT_CUSTOM(errMsgNoDrawCaller());
        }
        return *drawCaller_.value().get();
    }

protected:
    using LogComponent = IDrawComponent::LogComponent;

    void setRODesc(const RenderObjectDesc& roDesc) {
        roDesc_ = roDesc;
    }

    void resetRODesc() {
        roDesc_.reset();
    }

    void setDrawCaller(std::unique_ptr<BasicDrawCaller>&& drawCaller) {
        drawCaller_ = std::move(drawCaller);
    }

    void resetDrawCaller() {
        drawCaller_.reset();
    }

private:
    static constexpr const char* const errMsgNoRODesc() noexcept {
        return "RenderObjectDesc didn't have any value.\n"
            "Please check it's intialized incorrectly or has been reset unintentionally.\n";
    }

    static constexpr const char* const errMsgNoDrawCaller() noexcept {
        return "DrawCaller didn't have any value.\n"
            "Please check it's intialized incorrectly or has been reset unintentionally.\n";
    }

    std::optional< RenderObjectDesc > roDesc_;
    std::optional< std::unique_ptr<BasicDrawCaller> > drawCaller_;
};

// Prevent instantiation with general type T.
// DrawComponent has to be specialized.
// There's no common implementation between other type of DrawComponents,
// This class template stands for having constraint of naming.
// e.g. to enforce the class which is going to support IDrawComponent interface
// to have a predictable name DrawComponent<Box>,
// rather than unpredictable name BoxDrawComponent.
// Note: The specialization of this class template must inherit IDrawComponent.
template <class T>
class DrawComponent;

#endif  // __DrawComponent