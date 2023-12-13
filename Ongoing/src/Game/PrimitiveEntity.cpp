#include "Game/PrimitiveEntity.hpp"

std::vector<dx::XMMATRIX> PETransformCBuf::initialTransforms() {
    return std::vector<dx::XMMATRIX>{
        dx::XMMatrixTranspose(
            dx::XMMatrixIdentity()
            * dx::XMMatrixRotationY(0.7f)
            * dx::XMMatrixRotationX(-0.4f)
            * dx::XMMatrixTranslation( 0.f, 0.f, 4.f )
            * dx::XMMatrixPerspectiveLH( 1.f, 3.f/4.f, 0.5f, 10.f )
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

void PEDrawContext::drawCall(GFXPipeline& pipeline) const {
    // first adopt transform
    assert( mappedStorage_->get(IDTransCBuf_).has_value() );

    auto transCBuf = static_cast<PETransformCBuf*>(
        mappedStorage_->get(IDTransCBuf_).value()
    );

    transCBuf->dynamicUpdate(pipeline, [this](){ return trans_.data(); });
    // then draw
    DrawContextIndexed::indexedDrawCall(pipeline);
}