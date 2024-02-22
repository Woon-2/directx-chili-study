#ifndef __PSphere
#define __PSphere

#include "GFX/Core/Factory.hpp"
#include "GFX/PipelineObjects/IA.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"

#include <DirectXMath.h>
#include "GFX/Core/Namespaces.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>

namespace Primitives {

struct Sphere {
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;

    static constexpr auto defNTesselation = 16u;

    class SphereVertexBuffer : public VertexBuffer<MyVertex> {
    public:
        SphereVertexBuffer() = default;
        SphereVertexBuffer( GFXFactory factory,
            std::size_t nTesselationLat = defNTesselation,
            std::size_t nTesselationLong = defNTesselation
        ) : VertexBuffer<MyVertex>( factory,
                Sphere::modelPositions< std::vector<MyVertex> >(
                    nTesselationLat, nTesselationLong
                )
            ) {}
            
        static constexpr std::size_t size(
            std::size_t nTesselationLat = defNTesselation,
            std::size_t nTesselationLong = defNTesselation
        ) {
            return (nTesselationLat - 1) * nTesselationLong + 2u;
        }
    };

    class SphereIndexBuffer : public IndexBuffer<MyIndex> {
    public:
        SphereIndexBuffer() = default;
        SphereIndexBuffer( GFXFactory factory,
            std::size_t nTesselationLat = defNTesselation,
            std::size_t nTesselationLong = defNTesselation
        ) : IndexBuffer<MyIndex>( factory,
                Sphere::modelIndices< std::vector<MyIndex> >(
                    nTesselationLat, nTesselationLong
                )
            ) {}

        static constexpr std::size_t size(
            std::size_t nTesselationLat = defNTesselation,
            std::size_t nTesselationLong = defNTesselation
        ) {
            return (nTesselationLat - 1) * (nTesselationLong) * 6;
        }
    };

    template <std::ranges::contiguous_range VertexPosContainer>
    static VertexPosContainer modelPositions(
        std::size_t nTesselationLat = defNTesselation,
        std::size_t nTesselationLong = defNTesselation
    ) {
        if (nTesselationLat < 3 || nTesselationLong < 3) {
            throw GFX_EXCEPT_CUSTOM(
                "Sphere is not definable with nTesselation value less than 3.\n"
                "(When nTesselation is 3, it means base aspect is a triangle.)\n"
            );
        }

        constexpr auto pi = 3.14159f;
        constexpr float radius = 1.0f;
        using pos_type = typename VertexPosContainer::value_type;

        VertexPosContainer ret;
        reserve_if_possible( ret, SphereVertexBuffer::size(
            nTesselationLat, nTesselationLong
        ) );
        auto out = std::back_inserter(ret);

        const auto base = dx::XMVectorSet( 0.f, 0.f, radius, 0.f );
		const float lattitudeAngle = pi / nTesselationLat;
		const float longitudeAngle = 2.0f * pi / nTesselationLong;

        for (auto iLat = decltype(nTesselationLat)(1u); iLat < nTesselationLat; ++iLat) {
            // pre-calculate lattitude of the vertex for optimization
            auto vLatCalculated = dx::XMVector3Transform(
                base, dx::XMMatrixRotationX(lattitudeAngle * iLat)
            );

            for (auto iLong = decltype(nTesselationLong)(0); iLong < nTesselationLong; ++iLong) {
                auto v = dx::XMVector3Transform(
                    vLatCalculated, dx::XMMatrixRotationZ(longitudeAngle * iLong)
                );
                auto tmp = dx::XMFLOAT3();
                dx::XMStoreFloat3(&tmp, v);

                out = pos_type(tmp.x, tmp.y, tmp.z);
            }
        }

        // add the cap vertices
        auto tmpNorthPole = dx::XMFLOAT3();
        dx::XMStoreFloat3(&tmpNorthPole, base);
        out = pos_type(tmpNorthPole.x, tmpNorthPole.y, tmpNorthPole.z);

        auto tmpSouthPole = dx::XMFLOAT3();
        dx::XMStoreFloat3(&tmpSouthPole, dx::XMVectorNegate(base));
        out = pos_type(tmpSouthPole.x, tmpSouthPole.y, tmpSouthPole.z);

        return ret;
    }

    template <std::ranges::contiguous_range VertexIdxContainer>
    static VertexIdxContainer modelIndices(
         std::size_t nTesselationLat = defNTesselation,
        std::size_t nTesselationLong = defNTesselation
    ) {
        if (nTesselationLat < 3 || nTesselationLong < 3) {
            throw GFX_EXCEPT_CUSTOM(
                "Sphere is not definable with nTesselation value less than 3.\n"
                "(When nTesselation is 3, it means base aspect is a triangle.)\n"
            );
        }

        static constexpr auto pi = 3.14159f;
        using idx_type = typename VertexIdxContainer::value_type;

        const auto iNorthPole = (nTesselationLat - 1) * nTesselationLong;
        const auto iSouthPole = (nTesselationLat - 1) * nTesselationLong + 1u;

        VertexIdxContainer ret;
        reserve_if_possible( ret, SphereIndexBuffer::size(
            nTesselationLat, nTesselationLong
        ) );
        auto out = std::back_inserter(ret);

        auto dispatchIdx = [nTesselationLong](auto iLat, auto iLong) {
            return iLat * nTesselationLong + iLong;
        };

        for (auto iLat = decltype(nTesselationLat)(0); iLat < nTesselationLat - 2; ++iLat) {
            for (auto iLong = decltype(nTesselationLong)(0); iLong < nTesselationLong; ++iLong) {
                // make circular via modulo,
                // the last index should be the first index.
                const auto mod = nTesselationLong;

                out = idx_type( dispatchIdx(iLat, iLong) );
                out = idx_type( dispatchIdx(iLat + 1, iLong) );
                out = idx_type( dispatchIdx(iLat, (iLong + 1) % mod) );
                out = idx_type( dispatchIdx(iLat, (iLong + 1) % mod) );
                out = idx_type( dispatchIdx(iLat + 1, iLong) );
                out = idx_type( dispatchIdx(iLat + 1, (iLong + 1) % mod) );
            }
        }

        // cap fans
        for (auto iLong = decltype(nTesselationLong)(0); iLong < nTesselationLong; ++iLong) {
            // make circular via modulo,
            // the last index should be the first index.
            const auto mod = nTesselationLong;

            // north
            out = idx_type( iNorthPole );
            out = idx_type( dispatchIdx(0, iLong) );
            out = idx_type( dispatchIdx(0, (iLong + 1) % mod) );
            // south
            out = idx_type( dispatchIdx(nTesselationLat - 2, (iLong + 1) % mod) );
            out = idx_type( dispatchIdx(nTesselationLat - 2, iLong) );
            out = idx_type( iSouthPole );
        }

        return ret;
    }
};

} // namespace Primitives

#endif  // __PSphere