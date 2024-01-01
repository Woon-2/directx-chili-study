#ifndef __RCDrawComponent
#define __RCDrawComponent

#include "DrawComponent.hpp"

class Renderer;
class CameraVision;

class RCDrawCmp : public DynDrawCmpBase {
public:
    virtual void sync(const Renderer&) = 0;
    virtual void sync(const CameraVision&) = 0;

protected:
    using LogComponent = DynDrawCmpBase::LogComponent;
};

#endif  // __RCDrawComponent