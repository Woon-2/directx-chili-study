#ifndef __Cone
#define __Cone

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "DrawComponent.hpp"

#include "GFX/Primitives/Cone.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"

class Cone {

};

template <>
class DrawComponent<Cone> : public PEDrawComponent<
    Cone, Primitives::Cone::ConeVertexBuffer,
    Primitives::Cone::ConeIndexBuffer
> {
public:
    using MyBase = PEDrawComponent<
        Cone, Primitives::Cone::ConeVertexBuffer,
        Primitives::Cone::ConeIndexBuffer
    >;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd
    ) : MyBase(factory, pipeline, scene, wnd) {}

};

template<>
class Entity<Cone> : public PrimitiveEntity<Cone> {

};

#endif  // __Cone