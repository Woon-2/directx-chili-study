#include "Game/PrimitiveEntity.hpp"

#include <ranges>
#include <algorithm>
#include <numeric>

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