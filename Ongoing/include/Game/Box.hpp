#ifndef __Box
#define __Box

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "DrawComponent.hpp"

#include "GFX/Primitives/Cube.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"

class Box {

};

template <>
class DrawComponent<Box> : public PEDrawComponentIndexed<
    Box, Primitives::Cube::CubeVertexBuffer,
    Primitives::Cube::CubeIndexBuffer
> {
public:
    using MyBase = PEDrawComponentIndexed<
        Box, Primitives::Cube::CubeVertexBuffer,
        Primitives::Cube::CubeIndexBuffer
    >;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd
    ) : MyBase(factory, pipeline, scene, wnd) {}

};

template<>
class Entity<Box> : public PrimitiveEntity<Box> {

};

#endif  // __Box