#ifndef __DrawContext
#define __DrawContext

class GFXPipeline;

class IDrawContext {
public:
    virtual ~IDrawContext() = 0 {}

    virtual void beforeDrawCall(GFXPipeline& pipeline) {}
    virtual void afterDrawCall(GFXPipeline& pipeline) {}
};

#endif  // __DrawContext