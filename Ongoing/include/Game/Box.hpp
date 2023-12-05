#ifndef __Box
#define __Box

#include "Entity.hpp"
#include "Scene.hpp"
#include "RenderDesc.hpp"
#include "DrawComponent.hpp"
#include "InputSystem.hpp"
#include "InputComponent.hpp"
#include "GTransformComponent.hpp"

#include "GFX/Primitives/Cube.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/GraphicsStorage.hpp"
#include "GFX/Core/Pipeline.hpp"
#include "GFX/PipelineObjects/IA.hpp"

#include <vector>
#include <memory>

#include "GFX/Core/GraphicsNamespaces.hpp"

#include "PrimitiveEntity.hpp"

class Box {

};

template <>
class DrawComponent<Box> : public PEDrawComponent<
    Box, Cube::CubeVertexBuffer, Cube::CubeIndexBuffer
> {
public:
    using MyBase = PEDrawComponent<
        Box, Cube::CubeVertexBuffer, Cube::CubeIndexBuffer
    >;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd
    ) : MyBase(factory, pipeline, scene, wnd) {}

};

template<>
class MouseInputComponent<Box> : public IMouseInputComponent {
private:
    struct DeltaPosition {
        float dx;
        float dy;

        static const DeltaPosition from(const AppMousePoint& cur,
            const AppMousePoint& last
        ) {
            return DeltaPosition{
                .dx = cur.x - last.x,
                .dy = cur.y - last.y
            };
        }
    };
public:
    MouseInputComponent(const MousePointConverter& converter,
        const AppMousePoint& initialPos
    ) : curPos_(initialPos), lastPos_(initialPos), pConverter_(&converter) {}

    DeltaPosition deltaPos() const {
        return DeltaPosition::from(curPos_, lastPos_);
    }

    AppMousePoint pos() const noexcept {
        return curPos_;
    }
    
    void update();

    void receive(const Mouse::Event& ev) override;

private:
    AppMousePoint curPos_;
    AppMousePoint lastPos_;
    const MousePointConverter* pConverter_;
};

template<>
class Entity<Box> : public IEntity {
public:
    friend class Loader<Box>;

    Entity() = default;
    Entity(const Box& box)
        : box_(box), dc_(), ic_() {}

    void update(milliseconds elapsed) override;

    // ct stands for construct
    void ctDrawComponent(GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd
    );
    void ctInputComponent(const MousePointConverter& converter,
        const AppMousePoint& initialPos
    );
    void ctTransformComponent( Distribution& distRadius,
        Distribution& distCTP,  // chi, theta, phi
        Distribution& distDeltaCTP, // chi, theta, phi
        Distribution& distDeltaRTY   // roll, yaw, pitch
    );

    Loader<Box> loader() const noexcept;

private:
    std::shared_ptr< DrawComponent<Box> >
    drawComponent() const noexcept {
        return dc_;
    }

    std::shared_ptr< MouseInputComponent<Box> >
    inputComponent() const noexcept {
        return ic_;
    }

    std::shared_ptr<BasicTransformComponent>
    transformComponent() const noexcept {
        return tc_;
    }

    Box box_;
    std::shared_ptr< DrawComponent<Box> > dc_;
    std::shared_ptr< MouseInputComponent<Box> > ic_;
    std::shared_ptr< GTransformComponent > tc_;
};

template<>
class Loader<Box> {
public:
    Loader(const Entity<Box>& entity)
        : entity_(entity) {}

    Loader(const Loader&) = delete;
    Loader& operator=(const Loader&) = delete;
    Loader(Loader&&) = delete;
    Loader& operator=(Loader&&) = delete;

    void loadAt(Scene& scene) {
        scene.addDrawComponent(entity_.drawComponent());
    }

    template <class CharT>
    void loadAt(InputSystem<CharT>& inputSystem) {
        inputSystem.setListner(entity_.inputComponent());
    }

private:
    const Entity<Box>& entity_;
};

#endif