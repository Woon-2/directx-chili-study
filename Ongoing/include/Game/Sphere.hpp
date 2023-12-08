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
        Scene& scene, const ChiliWindow& wnd
    ) : MyBase(factory, pipeline, scene, wnd) {}

};

template<>
class Entity<Sphere> : public PrimitiveEntity<Sphere> {

};

#endif  // __Sphere