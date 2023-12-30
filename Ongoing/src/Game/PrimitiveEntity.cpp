#include "Game/PrimitiveEntity.hpp"

#include <ranges>
#include <algorithm>
#include <numeric>

void MapTransformGPU::beforeDrawCall(GFXPipeline& pipeline) {
    assert( mappedStorage_->get(IDTransCBuf_).has_value() );

    auto transCBuf = static_cast<PETransformCBuf*>(
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

std::vector<dx::XMMATRIX> PETransformCBuf::initialTransforms() {
    return std::vector<dx::XMMATRIX>{
        dx::XMMatrixTranspose(
            dx::XMMatrixIdentity()
        )
    };
}

std::vector<PEFaceColorData> PEIndexedColorCBuf::initialColors() {
    return std::vector<PEFaceColorData>{{
        GFXColor{1.f, 0.f, 1.f, 1.f},
        GFXColor{1.f, 0.f, 0.f, 1.f},
        GFXColor{0.f, 1.f, 0.f, 1.f},
        GFXColor{0.f, 0.f, 1.f, 1.f},
        GFXColor{1.f, 1.f, 0.f, 1.f},
        GFXColor{0.f, 1.f, 1.f, 1.f}
    }};
}

std::vector<GFXColor> PEColorBuffer::makeRandom(std::size_t size) {
    auto dist = std::uniform_real_distribution<float>(0.f, 1.f);
    auto ret = std::vector<GFXColor>();
    ret.reserve(size);

    while (size--) {
        ret.push_back( GFXColor{
            .r = dist(rng), .g = dist(rng), .b = dist(rng), .a = dist(rng)
        } );
    }

    return ret;
}