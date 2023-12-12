#ifndef __Plane
#define __Plane

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "DrawComponent.hpp"

#include "GFX/Primitives/Plane.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"

class Plane {

};

template <>
class DrawComponent<Plane> : public PEDrawComponentIndexed<
    Plane, Primitives::Plane::PlaneVertexBuffer,
    Primitives::Plane::PlaneIndexBuffer
> {
public:
    using MyBase = PEDrawComponentIndexed<
        Plane, Primitives::Plane::PlaneVertexBuffer,
        Primitives::Plane::PlaneIndexBuffer
    >;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd,
        std::size_t nTesselationX = Primitives::Plane::defNTesselation,
        std::size_t nTesselationY = Primitives::Plane::defNTesselation
    ) : MyBase(factory, pipeline, scene, wnd,
        nTesselationX, nTesselationY
    ) {}

};

template<>
class Entity<Plane> : public PrimitiveEntity<Plane> {

};

#endif  // __Plane