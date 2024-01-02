#ifndef __TransformDrawContexts
#define __TransformDrawContexts

#include "GFX/PipelineObjects/DrawContext.hpp"
#include "Transform.hpp"

#include "GFX/Core/GraphicsStorage.hpp"
#include "GFX/Core/Pipeline.hpp"

#include "GFX/Core/GraphicsNamespaces.hpp"

#include <optional>

class MapTransformGPU : public IDrawContext {
public:
    using MatrixType = dx::XMMATRIX;

    MapTransformGPU(GFXStorage& mappedStorage)
        : transform_(), mappedStorage_(&mappedStorage),
        IDTransCBuf_() {}

    void beforeDrawCall(GFXPipeline& pipeline) override;
    void setTCBufID(GFXStorage::ID id) {
        IDTransCBuf_ = id;
    }
    void update(Transform transform) {
        transform_ = transform;
    }

    void pushBackApplyee(Transform transform) {
        applyees_.push_back(transform);
    }

    void popBackApplyee() {
        applyees_.pop_back();
    }

private:
    Transform transform_;
    std::vector<Transform> applyees_;
    GFXStorage* mappedStorage_;
    std::optional<GFXStorage::ID> IDTransCBuf_;
};

class ApplyTransform : public IDrawContext {
public:
    ApplyTransform(MapTransformGPU& mapper)
        : transform_(), mapper_(mapper) {}

    void setTransform(Transform transform) {
        transform_ = transform;
    }

    void beforeDrawCall(GFXPipeline& pipeline) override;
    void afterDrawCall(GFXPipeline& pipeline) override;

private:
    Transform transform_;
    MapTransformGPU& mapper_;
};

#endif  // __TransformDrawContexts