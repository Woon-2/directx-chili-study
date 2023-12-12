#ifndef __Prism
#define __Prism

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "DrawComponent.hpp"

#include "GFX/Primitives/Prism.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"

class Prism {

};

template <>
class DrawComponent<Prism> : public PEDrawComponentIndexed<
    Prism, Primitives::Prism::PrismVertexBuffer,
    Primitives::Prism::PrismIndexBuffer
> {
public:
    using MyBase = PEDrawComponentIndexed<
        Prism, Primitives::Prism::PrismVertexBuffer,
        Primitives::Prism::PrismIndexBuffer
    >;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd,
        std::size_t nTesselation = Primitives::Prism::defNTesselation
    ) : MyBase(factory, pipeline, scene, wnd, nTesselation) {}

};

template<>
class Entity<Prism> : public PrimitiveEntity<Prism> {

};

#endif  // __Prism