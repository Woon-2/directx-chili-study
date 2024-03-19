#ifndef __PPrism
#define __PPrism

#include "GFX/Core/Factory.hpp"
#include "GFX/PipelineObjects/IA.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"

#include <DirectXMath.h>
#include "GFX/Core/Namespaces.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>

namespace gfx {
namespace Primitives {

struct Prism {
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;

    static constexpr auto defNTesselation = 16u;

    class PrismVertexBuffer : public po::VertexBuffer<MyVertex> {
    public:
        PrismVertexBuffer() = default;
        PrismVertexBuffer( GFXFactory factory,
            std::size_t nTesselation = defNTesselation
        ) : po::VertexBuffer<MyVertex>( factory,
                Prism::modelPositions< std::vector<MyVertex> >( nTesselation )
            ) {}
            
        static constexpr std::size_t size(
            std::size_t nTesselation = defNTesselation
        ) {
            return 2u + 2u * nTesselation;
        }
    };

    class PrismIndexBuffer : public po::IndexBuffer<MyIndex> {
    public:
        PrismIndexBuffer() = default;
        PrismIndexBuffer( GFXFactory factory,
            std::size_t nTesselation = defNTesselation
        ) : po::IndexBuffer<MyIndex>( factory,
                Prism::modelIndices< std::vector<MyIndex> >(nTesselation)
            ) {}

        static constexpr std::size_t size(
            std::size_t nTesselation = defNTesselation
        ) {
            return 6u * (2u * nTesselation);
        }
    };

    template <std::ranges::contiguous_range VertexPosContainer>
    static VertexPosContainer modelPositions(
        std::size_t nTesselation = defNTesselation
    ) {
        if (nTesselation < 3) {
            throw GFX_EXCEPT_CUSTOM(
                "Prism is not definable with nTesselation value less than 3.\n"
                "(When nTesselation is 3, it means base aspect is a triangle.)\n"
            );
        }

        constexpr auto pi = 3.14159f;
        constexpr auto radius = 1.f;
        constexpr auto height = 1.f;
        using pos_type = typename VertexPosContainer::value_type;

        VertexPosContainer ret;
        reserve_if_possible( ret, PrismVertexBuffer::size(nTesselation) );
        auto out = std::back_inserter(ret);

        // near center
        out = pos_type( 0.f, 0.f, -height );
        // far center
        out = pos_type( 0.f, 0.f, height );

        const auto base = dx::XMVectorSet( radius, 0.0f, -height, 0.0f );
        const auto offset = dx::XMVectorSet( 0.0f, 0.0f, 2*height, 0.0f );
		const float longitudeAngle = 2.0f * pi / nTesselation;

        // base vertices
        for (auto iLong = decltype(nTesselation)(0); iLong < nTesselation; ++iLong) {
            // near base
            auto nb = dx::XMVector3Transform(
                base, dx::XMMatrixRotationZ(longitudeAngle * iLong)
            );
            auto tmp = dx::XMFLOAT3();
            dx::XMStoreFloat3(&tmp, nb);

            out = pos_type(tmp.x, tmp.y, tmp.z);

            // far base
            auto fb = dx::XMVector3Transform(
                base, dx::XMMatrixRotationZ(longitudeAngle * iLong)
            );
            fb = dx::XMVectorAdd(fb, offset);
            dx::XMStoreFloat3(&tmp, fb);

            out = pos_type(tmp.x, tmp.y, tmp.z);
        }

        return ret;
    }

    template <std::ranges::contiguous_range VertexIdxContainer>
    static VertexIdxContainer modelIndices(
        std::size_t nTesselation = defNTesselation
    ) {
        if (nTesselation < 3) {
            throw GFX_EXCEPT_CUSTOM(
                "Prism is not definable with nTesselation value less than 3.\n"
                "(When nTesselation is 3, it means base aspect is a triangle.)\n"
            );
        }

        static constexpr auto pi = 3.14159f;
        using idx_type = typename VertexIdxContainer::value_type;

        const auto iNearCenter = idx_type(0u);
        const auto iFarCenter = idx_type(1u);

        VertexIdxContainer ret;
        reserve_if_possible( ret, PrismIndexBuffer::size(nTesselation) );
        auto out = std::back_inserter(ret);

        // side indices
        for (auto iLong = decltype(nTesselation)(0); iLong < nTesselation; ++iLong) {
            const auto i = iLong * 2;
            // make circular via modulo,
            // the last index should be the first index.
            const auto mod = nTesselation * 2;

            out = idx_type(i + 2);
            out = idx_type( (i + 2) % mod + 2 );
            out = idx_type( (i + 2) + 1 );
            out = idx_type( (i + 2) % mod + 2 );
            out = idx_type( ((i + 2) + 1) % mod + 2 );
            out = idx_type( (i + 2) + 1 );
        }

        // base indices
        for (auto iLong = decltype(nTesselation)(0); iLong < nTesselation; ++iLong) {
            const auto i = iLong * 2;
            const auto mod = nTesselation * 2;
            out = idx_type(i + 2);
            out = idx_type(iNearCenter);   // near center
            out = idx_type( (i + 2) % mod + 2 );
            out = idx_type(iFarCenter);   // far center
            out = idx_type( (i + 2) + 1 );
            out = idx_type( ((i + 2) + 1) % mod + 2 );
        }

        return ret;
    }
};

}   // namespace gfx::Primitives
}   // namespace gfx

#endif  // __PPrism