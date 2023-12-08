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
class DrawComponent<Plane> : public PEDrawComponent<
    Plane, Primitives::Plane::PlaneVertexBuffer,
    Primitives::Plane::PlaneIndexBuffer
> {
public:
    using MyBase = PEDrawComponent<
        Plane, Primitives::Plane::PlaneVertexBuffer,
        Primitives::Plane::PlaneIndexBuffer
    >;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd
    ) : MyBase(factory, pipeline, scene, wnd) {}

};

template<>
class Entity<Plane> : public PrimitiveEntity<Plane> {

};

#endif  // __Plane