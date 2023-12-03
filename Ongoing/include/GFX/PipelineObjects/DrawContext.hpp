#ifndef __DrawContext
#define __DrawContext

#include <d3d11.h>

class GFXPipeline;

class IDrawContext {
public:
    friend class GFXPipeline;

private:
    virtual void drawCall(GFXPipeline& pipeline) const = 0;
};

class DrawContextIndexed : public IDrawContext {
public:
    DrawContextIndexed() = default;
    DrawContextIndexed(UINT numIndex, UINT startIndexLocation,
        INT baseVertexLocation
    ) : numIndex_(numIndex), startIndexLocation_(startIndexLocation),
        baseVertexLocation_(baseVertexLocation) {}

    void setNumIndex(UINT numIndex) noexcept {
        numIndex_ = numIndex;
    }

    UINT numIndex() const noexcept {
        return numIndex_;
    }

    void setStartIndexLocation(UINT startIndexLocation) noexcept {
        startIndexLocation_ = startIndexLocation;
    }

    UINT startIndexLocation() const noexcept {
        return startIndexLocation_;
    }

    void setbaseVertexLocation(UINT baseVertexLocation) noexcept {
        baseVertexLocation_ = baseVertexLocation;
    }

    INT baseVertexLocation() const noexcept {
        return baseVertexLocation_;
    }

private:
    void drawCall(GFXPipeline& pipeline) const override;

    UINT numIndex_;
    UINT startIndexLocation_;
    INT baseVertexLocation_;
};

#endif  // __DrawContext