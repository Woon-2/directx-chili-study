#ifndef __PCube
#define __PCube

#include "GFX/Core/GFXFactory.hpp"
#include "GFX/PipelineObjects/IA.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>
#include <array>

namespace Primitives {

struct Cube {
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;

    static constexpr auto side = 0.5f;

    class CubeVertexBuffer : public VertexBuffer<MyVertex> {
    public:
        CubeVertexBuffer() = default;
        CubeVertexBuffer(GFXFactory factory)
            : VertexBuffer<MyVertex>( factory,
                Cube::modelPositions< std::vector<MyVertex> >()
            ) {}

        static constexpr std::size_t size() {
            return 8u;
        }
    };

    class CubeVertexBufferIndependent : public VertexBuffer<MyVertex> {
    public:
        CubeVertexBufferIndependent() = default;
        CubeVertexBufferIndependent(GFXFactory factory)
            : VertexBuffer<MyVertex>( factory,
                Cube::modelPositionsIndependent< std::vector<MyVertex> >()
            ) {}

        static constexpr std::size_t size() {
            return 36u;
        }
    };

    class CubeIndexBuffer : public IndexBuffer<MyIndex> {
    public:
        CubeIndexBuffer() = default;
        CubeIndexBuffer(GFXFactory factory)
            : IndexBuffer<MyIndex>( factory,
                Cube::modelIndices< std::vector<MyIndex> >()
            ) {}

        static constexpr std::size_t size() {
            return 36u;
        }
    };

    template <std::ranges::contiguous_range VertexPosContainer>
    static VertexPosContainer modelPositions() {
        using pos_type = typename VertexPosContainer::value_type;

        VertexPosContainer ret;
        reserve_if_possible( ret, 8u );
        auto out = std::back_inserter(ret);

		out = pos_type( -side,-side,-side ); // 0
		out = pos_type( side,-side,-side ); // 1
		out = pos_type( -side,side,-side ); // 2
		out = pos_type( side,side,-side ); // 3
		out = pos_type( -side,-side,side ); // 4
		out = pos_type( side,-side,side ); // 5
		out = pos_type( -side,side,side ); // 6
		out = pos_type( side,side,side ); // 7

        return ret;
    }

    template <std::ranges::contiguous_range VertexPosContainer>
    static VertexPosContainer modelPositionsIndependent() {
        using pos_type = typename VertexPosContainer::value_type;

        VertexPosContainer ret;
        reserve_if_possible( ret, 24u );
        auto out = std::back_inserter(ret);

        auto vertices = std::array<pos_type, 8>{
            pos_type( -side,-side,-side ), // 0
            pos_type( side,-side,-side ), // 1
            pos_type( -side,side,-side ), // 2
            pos_type( side,side,-side ), // 3
            pos_type( -side,-side,side ), // 4
            pos_type( side,-side,side ), // 5
            pos_type( -side,side,side ), // 6
            pos_type( side,side,side ) // 7
        };

        out = vertices[0]; out = vertices[2]; out = vertices[1]; out = vertices[2]; out = vertices[3]; out = vertices[1];
        out = vertices[1]; out = vertices[3]; out = vertices[5]; out = vertices[3]; out = vertices[7]; out = vertices[5];
        out = vertices[2]; out = vertices[6]; out = vertices[3]; out = vertices[3]; out = vertices[6]; out = vertices[7];
        out = vertices[4]; out = vertices[5]; out = vertices[7]; out = vertices[4]; out = vertices[7]; out = vertices[6];
        out = vertices[0]; out = vertices[4]; out = vertices[2]; out = vertices[2]; out = vertices[4]; out = vertices[6];
        out = vertices[0]; out = vertices[1]; out = vertices[4]; out = vertices[1]; out = vertices[5]; out = vertices[4];

        return ret;
    }

    template <std::ranges::contiguous_range VertexIdxContainer>
    static VertexIdxContainer modelIndices() {
        using idx_type = typename VertexIdxContainer::value_type;

        VertexIdxContainer ret;
        reserve_if_possible( ret, 36u );
        auto out = std::back_inserter(ret);

        // each line represents a face of a cube.
        out = 0u; out = 2u; out = 1u; out = 2u; out = 3u; out = 1u;
        out = 1u; out = 3u; out = 5u; out = 3u; out = 7u; out = 5u;
        out = 2u; out = 6u; out = 3u; out = 3u; out = 6u; out = 7u;
        out = 4u; out = 5u; out = 7u; out = 4u; out = 7u; out = 6u;
        out = 0u; out = 4u; out = 2u; out = 2u; out = 4u; out = 6u;
        out = 0u; out = 1u; out = 4u; out = 1u; out = 5u; out = 4u;

        return ret;
    }
};

}   // namespace Primitives

#endif  // __PCube