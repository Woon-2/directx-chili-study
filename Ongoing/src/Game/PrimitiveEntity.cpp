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
        gfx::GFXColor{1.f, 0.f, 1.f, 1.f},
        gfx::GFXColor{1.f, 0.f, 0.f, 1.f},
        gfx::GFXColor{0.f, 1.f, 0.f, 1.f},
        gfx::GFXColor{0.f, 0.f, 1.f, 1.f},
        gfx::GFXColor{1.f, 1.f, 0.f, 1.f},
        gfx::GFXColor{0.f, 1.f, 1.f, 1.f}
    }};
}

std::vector<gfx::GFXColor> PEColorBuffer::makeRandom(std::size_t size) {
    auto dist = std::uniform_real_distribution<float>(0.f, 1.f);
    auto ret = std::vector<gfx::GFXColor>();
    ret.reserve(size);

    while (size--) {
        ret.push_back( gfx::GFXColor{
            .r = dist(rng), .g = dist(rng), .b = dist(rng), .a = dist(rng)
        } );
    }

    return ret;
}