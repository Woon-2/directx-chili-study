#ifndef __PPlain
#define __PPlain

#include "GFX/Core/GFXFactory.hpp"
#include "GFX/PipelineObjects/IA.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>

#include <cassert>

namespace Primitives {

struct Plane {
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;

    static constexpr auto defNTesselation = 16u;

    class PlaneVertexBuffer : public VertexBuffer<MyVertex> {
    public:
        PlaneVertexBuffer() = default;
        PlaneVertexBuffer( GFXFactory factory,
            std::size_t nTesselationX = defNTesselation,
            std::size_t nTesselationY = defNTesselation
        ) : VertexBuffer<MyVertex>( factory,
                Plane::modelPositions< std::vector<MyVertex> >(
                    nTesselationX, nTesselationY
                )
            ) {}
            
        static constexpr std::size_t size(
            std::size_t nTesselationX = defNTesselation,
            std::size_t nTesselationY = defNTesselation
        ) {
            return (nTesselationX + 1) * (nTesselationY + 1);
        }
    };

    class PlaneIndexBuffer : public IndexBuffer<MyIndex> {
    public:
        PlaneIndexBuffer() = default;
        PlaneIndexBuffer( GFXFactory factory,
            std::size_t nTesselationX = defNTesselation,
            std::size_t nTesselationY = defNTesselation
        ) : IndexBuffer<MyIndex>( factory,
                Plane::modelIndices< std::vector<MyIndex> >(
                    nTesselationX, nTesselationY
                )
            ) {}

        static constexpr std::size_t size(
            std::size_t nTesselationX = defNTesselation,
            std::size_t nTesselationY = defNTesselation
        ) {
            return (nTesselationX + 1) * (nTesselationY + 1) * 6;
        }
    };

    template <std::ranges::contiguous_range VertexPosContainer>
    static VertexPosContainer modelPositions(
        std::size_t nTesselationX = defNTesselation,
        std::size_t nTesselationY = defNTesselation
    ) {
        assert(nTesselationX >= 1);
        assert(nTesselationY >= 1);

        using pos_type = typename VertexPosContainer::value_type;

        VertexPosContainer ret;
        auto out = std::back_inserter(ret);

        static constexpr auto width = 2.f;
        static constexpr auto height = 2.f;
        const auto nVerticesX = nTesselationX + 1;
        const auto nVerticesY = nTesselationY + 1;

        const auto left = -width / 2.0f;
        const auto bottom = -height / 2.0f;
        const auto divisionSizeX = width / static_cast<decltype(width)>(nTesselationX);
        const auto divisionSizeY = height / static_cast<decltype(width)>(nTesselationY);

        for ( auto row = decltype(nVerticesY)(0); row < nVerticesY; ++row ) {
            const auto posY = bottom + row * divisionSizeY;

            for ( auto col = decltype(nVerticesX)(0); col < nVerticesX; ++col ) {
                const auto posX = left + col * divisionSizeX;

                out = pos_type(posX, posY, 0.f);
            }
        }

        return ret;
    }

    template <std::ranges::contiguous_range VertexIdxContainer>
    static VertexIdxContainer modelIndices(
        std::size_t nTesselationX = defNTesselation,
        std::size_t nTesselationY = defNTesselation
    ) {
        assert(nTesselationX >= 1);
        assert(nTesselationY >= 1);

        using idx_type = typename VertexIdxContainer::value_type;

        VertexIdxContainer ret;
        auto out = std::back_inserter(ret);

        const auto nVerticesX = nTesselationX + 1;

        auto dispatchIdx = [nVerticesX](auto row, auto col) {
            return row * nVerticesX + col;
        };

        for (auto row = decltype(nTesselationY)(0); row < nTesselationY; ++row) {
            for ( auto col = decltype(nTesselationX)(0); col < nTesselationX; ++col) {
                out = idx_type( dispatchIdx(row, col) );
                out = idx_type( dispatchIdx(row + 1u, col) );
                out = idx_type( dispatchIdx(row, col + 1u) );
                out = idx_type( dispatchIdx(row, col + 1u) );
                out = idx_type( dispatchIdx(row + 1u, col) );
                out = idx_type( dispatchIdx(row + 1u, col + 1u) );
            }
        }

        return ret;
    }
};

}   // namespace Primitives

#endif  // __PPlain