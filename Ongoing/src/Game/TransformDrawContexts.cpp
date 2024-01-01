#include "Game/TransformDrawContexts.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"

#include <numeric>

void MapTransformGPU::beforeDrawCall(GFXPipeline& pipeline) {
    assert( mappedStorage_->get(IDTransCBuf_).has_value() );

    auto transCBuf = static_cast< CBuffer<MatrixType>* >(
        mappedStorage_->get(IDTransCBuf_).value()
    );

    // to avoid dangling pointer issue
    // caused by following lambda returning the raw address of it,
    // accumulated transform should be declared here.
    auto accumulated = Transform();

    transCBuf->dynamicUpdate( pipeline, [this, &accumulated](){
        accumulated = std::accumulate( applyees_.begin(), applyees_.end(),
            transform_, [](Transform lhs, const Transform& rhs) {
                return lhs * rhs;
            }
        );

        accumulated = accumulated.transpose();
        return accumulated.data();
    } );
}

void ApplyTransform::beforeDrawCall(GFXPipeline& pipeline) {
    mapper_.pushBackApplyee(transform_);
}

void ApplyTransform::afterDrawCall(GFXPipeline& pipeline) {
    mapper_.popBackApplyee();
}