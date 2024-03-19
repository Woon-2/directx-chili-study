#ifndef __RCDrawComponent
#define __RCDrawComponent

#include "DrawComponent.hpp"

namespace gfx {
namespace scenery {

class Renderer;
class CameraVision;

class RCDrawCmp : public DynDrawCmpBase {
public:
    virtual void sync(const Renderer&) = 0;
    virtual void sync(const CameraVision&) = 0;

protected:
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
    using LogComponent = DynDrawCmpBase::LogComponent;
#endif
};

}  // namespace gfx::scenery
}  // namespace gfx

#endif  // __RCDrawComponent