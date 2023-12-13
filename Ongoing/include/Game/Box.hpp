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
class DrawComponent<Box> : public PEDrawComponent<
    Box, Primitives::Cube::CubeVertexBuffer,
    Primitives::Cube::CubeIndexBuffer
> {
public:
    using MyBase = PEDrawComponent<
        Box, Primitives::Cube::CubeVertexBuffer,
        Primitives::Cube::CubeIndexBuffer
    >;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        GFXStorage& storage, const ChiliWindow& wnd
    ) : MyBase(factory, pipeline, storage, wnd) {}

};

template<>
class Entity<Box> : public PrimitiveEntity<Box> {

};

#endif  // __Box