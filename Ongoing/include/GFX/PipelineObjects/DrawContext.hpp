#ifndef __DrawContext
#define __DrawContext

namespace gfx {

class GFXPipeline;

namespace po {

class IDrawContext {
public:
    virtual ~IDrawContext() = 0 {}

    virtual void beforeDrawCall(GFXPipeline& pipeline) {}
    virtual void afterDrawCall(GFXPipeline& pipeline) {}
};

}   // namespace gfx::po
}   // namespace gfx

#endif  // __DrawContext