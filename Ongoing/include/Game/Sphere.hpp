#ifndef __Sphere
#define __Sphere

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "DrawComponent.hpp"

#include "GFX/Primitives/Sphere.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"

class Sphere {

};


template <>
class DrawComponent<Sphere> : public PEDrawComponent<
    Sphere, Primitives::Sphere::SphereVertexBuffer,
    Primitives::Sphere::SphereIndexBuffer
> {
public:
    using MyBase = PEDrawComponent<
        Sphere, Primitives::Sphere::SphereVertexBuffer,
        Primitives::Sphere::SphereIndexBuffer
    >;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        GFXStorage& storage, const ChiliWindow& wnd,
        std::size_t nTesselationLat = Primitives::Sphere::defNTesselation,
        std::size_t nTesselationLong = Primitives::Sphere::defNTesselation
    ) : MyBase(factory, pipeline, storage, wnd,
        nTesselationLat, nTesselationLong
    ) {}

};

template<>
class Entity<Sphere> : public PrimitiveEntity<Sphere> {

};

#endif  // __Sphere