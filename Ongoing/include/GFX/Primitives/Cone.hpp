#ifndef __PCone
#define __PCone

#include "GFX/Core/GFXFactory.hpp"
#include "GFX/PipelineObjects/IA.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"

#include <DirectXMath.h>
#include "GFX/Core/GraphicsException.hpp"
#include "GFX/Core/GraphicsNamespaces.hpp"

#include <ranges>
#include <iterator>
#include <algorithm>

namespace Primitives {

struct Cone {
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;

    static constexpr auto defNTesselation = 16u;

    class ConeVertexBuffer : public VertexBuffer<MyVertex> {
    public:
        ConeVertexBuffer() = default;
        ConeVertexBuffer( GFXFactory factory,
            std::size_t nTesselation = defNTesselation
        ) : VertexBuffer<MyVertex>( factory,
                Cone::modelPositions< std::vector<MyVertex> >(nTesselation)
            ) {}
            
        static constexpr std::size_t size(
            std::size_t nTesselation = defNTesselation
        ) {
            return nTesselation + 2u;
        }
    };

    class ConeIndexBuffer : public IndexBuffer<MyIndex> {
    public:
        ConeIndexBuffer() = default;
        ConeIndexBuffer( GFXFactory factory,
            std::size_t nTesselation = defNTesselation
        ) : IndexBuffer<MyIndex>( factory,
                Cone::modelIndices< std::vector<MyIndex> >(nTesselation)
            ) {}

        static constexpr std::size_t size(
            std::size_t nTesselation = defNTesselation
        ) {
            return 6u * nTesselation;
        }
    };

    template <std::ranges::contiguous_range VertexPosContainer>
    static VertexPosContainer modelPositions(
        std::size_t nTesselation = defNTesselation
    ) {
        if (nTesselation < 3) {
            throw GFX_EXCEPT_CUSTOM(
                "Cone is not definable with nTesselation value less than 3.\n"
                "(When nTesselation is 3, it means base aspect is a triangle.)\n"
            );
        }

        static constexpr auto pi = 3.14159f;
        using pos_type = typename VertexPosContainer::value_type;

        VertexPosContainer ret;
        auto out = std::back_inserter(ret);

        const auto base = dx::XMVectorSet( 1.0f,0.0f,-1.0f,0.0f );
		const float longitudeAngle = 2.0f * pi / nTesselation;

        // base vertices
		for( auto iLong = decltype(nTesselation)(0);
            iLong < nTesselation; iLong++
        ) {
			auto v = dx::XMVector3Transform( 
				base,
				dx::XMMatrixRotationZ( longitudeAngle * iLong )
			);
			auto tmp = dx::XMFLOAT3();
            dx::XMStoreFloat3( &tmp, v );
            
            out = pos_type(tmp.x, tmp.y, tmp.z);
		}

		// the center
		out = pos_type(0.0f,0.0f,-1.0f);

		// the tip :darkness:
		out = pos_type(0.0f,0.0f,1.0f);

        return ret;
    }

    template <std::ranges::contiguous_range VertexIdxContainer>
    static VertexIdxContainer modelIndices(
        std::size_t nTesselation = defNTesselation
    ) {
        if (nTesselation < 3) {
            throw GFX_EXCEPT_CUSTOM(
                "Cone is not definable with nTesselation value less than 3.\n"
                "(When nTesselation is 3, it means base aspect is a triangle.)\n"
            );
        }

        static constexpr auto pi = 3.14159f;
        using idx_type = typename VertexIdxContainer::value_type;

        VertexIdxContainer ret;
        auto out = std::back_inserter(ret);

        // base indices
		for( auto iLong = decltype(nTesselation)(0);
            iLong < nTesselation; ++iLong
        ) {
			out = idx_type( nTesselation );
			out = idx_type( (iLong + 1) % nTesselation );
			out = idx_type( iLong );
		}

		// cone indices
		for( auto iLong = decltype(nTesselation)(0);
            iLong < nTesselation; ++iLong
        ) {
			out = idx_type( iLong );
			out = idx_type( (iLong + 1) % nTesselation );
			out = idx_type( nTesselation + 1 );
		}

        return ret;
    }
};

}   // namespace Primitives

#endif  // __PCone