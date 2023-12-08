#ifndef __PPrism
#define __PPrism

#include "GFX/Core/GFXFactory.hpp"
#include "GFX/PipelineObjects/IA.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"

#include <DirectXMath.h>
#include "GFX/Core/GraphicsNamespaces.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>

#include <cassert>

namespace Primitives {

struct Prism {
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;

    static constexpr auto defNTesselation = 16u;

    class PrismVertexBuffer : public VertexBuffer<MyVertex> {
    public:
        PrismVertexBuffer() = default;
        PrismVertexBuffer( GFXFactory factory,
            std::size_t nTesselation = defNTesselation
        ) : VertexBuffer<MyVertex>( factory,
                Prism::modelPositions< std::vector<MyVertex> >( nTesselation )
            ) {}
            
        static constexpr std::size_t size(
            std::size_t nTesselation = defNTesselation
        ) {
            return 2u + 2u * nTesselation;
        }
    };

    class PrismIndexBuffer : public IndexBuffer<MyIndex> {
    public:
        PrismIndexBuffer() = default;
        PrismIndexBuffer( GFXFactory factory,
            std::size_t nTesselation = defNTesselation
        ) : IndexBuffer<MyIndex>( factory,
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
        // Base aspect must have area,
        // when nTesselation is 3, it means base aspect is a triangle.
        assert( nTesselation >= 3 );

        constexpr auto pi = 3.14159f;
        constexpr auto radius = 1.f;
        constexpr auto height = 1.f;
        using pos_type = typename VertexPosContainer::value_type;

        VertexPosContainer ret;
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
        // Base aspect must have area,
        // when nTesselation is 3, it means base aspect is a triangle.
        assert( nTesselation >= 3 );

        static constexpr auto pi = 3.14159f;
        using idx_type = typename VertexIdxContainer::value_type;

        const auto iNearCenter = idx_type(0u);
        const auto iFarCenter = idx_type(1u);

        VertexIdxContainer ret;
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

}   // namespace Primitives

#endif  // __PPrism