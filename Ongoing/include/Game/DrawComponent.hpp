#ifndef __DrawComponent
#define __DrawComponent

#define ACTIVATE_DRAWCOMPONENT_LOG

#include "RenderObjectDesc.hpp"

#include "GFXCMDLogger.hpp"

class Renderer;
class BasicDrawCaller;
class CameraVision;

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

    private:
        const void* logSrc_;
        bool bLogEnabled_;
    };
#endif  // ACTIVATE_DRAWCOMPONENT_LOG
public:
    virtual ~IDrawComponent() {}

    virtual const RenderObjectDesc renderObjectDesc() const = 0;
    virtual BasicDrawCaller* drawCaller() = 0;
    virtual const BasicDrawCaller* drawCaller() const = 0;
    virtual void sync(const Renderer& pRenderer) = 0;
    virtual void sync(const CameraVision& v) {}

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