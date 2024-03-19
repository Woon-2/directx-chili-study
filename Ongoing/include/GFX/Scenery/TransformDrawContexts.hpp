#ifndef __TransformDrawContexts
#define __TransformDrawContexts

#include "GFX/PipelineObjects/DrawContext.hpp"
#include "GFX/Core/Transform.hpp"

#include "GFX/Core/Storage.hpp"
#include "GFX/Core/Pipeline.hpp"

#include "GFX/Core/Namespaces.hpp"

#include <optional>

namespace gfx {
namespace scenery {

class MapTransformGPU : public po::IDrawContext {
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

class ApplyTransform : public po::IDrawContext {
public:
    ApplyTransform(MapTransformGPU& mapper)
        : transform_(), mapper_(&mapper) {}

    void setTransform(Transform transform) {
        transform_ = transform;
    }

    void beforeDrawCall(GFXPipeline& pipeline) override;
    void afterDrawCall(GFXPipeline& pipeline) override;

    void linkMapper(MapTransformGPU& mapper) {
        mapper_ = &mapper;
    }

    void swap(ApplyTransform& ohter) noexcept {
        std::swap(transform_, ohter.transform_);
    }

private:
    Transform transform_;
    MapTransformGPU* mapper_;
};

}  // namespace gfx::scenery
}  // namespace gfx

#endif  // __TransformDrawContexts