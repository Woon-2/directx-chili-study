#ifndef __Box
#define __Box

#include "Entity.hpp"
#include "Scene.hpp"
#include "RenderDesc.hpp"
#include "DrawComponent.hpp"
#include "InputSystem.hpp"
#include "InputComponent.hpp"
#include "Transform.hpp"

#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/Pipeline.hpp"
#include "GFX/PipelineObjects/IA.hpp"

#include <vector>

#include "GFX/Core/GraphicsNamespaces.hpp"

class Box {

};

template <>
class DrawComponent<Box> : public IDrawComponent {
public:
    using MyType = DrawComponent<Box>;
    using MyVertex = GFXVertex;
    using MyIndex = GFXIndex;

    struct MyColor {
        float r;
        float g;
        float b;
        float a;
    };

    struct MyConstantBufferColor {
        MyColor faceColors[6];
    };

    class MyVertexBuffer;
    class MyIndexBuffer;
    class MyTopology;
    class MyTransform;
    class MyColorBuffer;
    class MyVertexShader;
    class MyPixelShader;
    class MyViewport;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd
    );

    void update(const Transform trans);

    const RenderDesc renderDesc() const override {
        return RenderDesc{
            .header = {
                .typeID = typeid(Box),
                .vertexShaderID = IDVertexShader_,
                .pixelShaderID = IDPixelShader_
            },
            .IDs = { IDVertexShader_, IDPixelShader_,
                IDVertexBuffer_, IDIndexBuffer_, IDTopology_,
                IDViewport_, IDTransform_, IDColor_
            }
        };
    }

    const IDrawContext* drawContext() const override {
        return &drawContext_;
    }

    IDrawContext* drawContext() override {
        return &drawContext_;
    }

private:
    DrawContextIndexed drawContext_;
    GFXPipeline pipeline_;
    Scene* pScene_;
    GFXStorage::ID IDVertexShader_;
    GFXStorage::ID IDPixelShader_;
    GFXStorage::ID IDVertexBuffer_;
    GFXStorage::ID IDIndexBuffer_;
    GFXStorage::ID IDTopology_;
    GFXStorage::ID IDViewport_;
    GFXStorage::ID IDTransform_;
    GFXStorage::ID IDColor_;
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
    Entity(const Box& box) noexcept
        : box_(box), dc_(), ic_() {}

    void update(milliseconds elapsed) override;

    // ct stands for construct
    void ctDrawComponent(GFXFactory factory, GFXPipeline pipeline,
        Scene& scene, const ChiliWindow& wnd
    );
    void ctInputComponent(const MousePointConverter& converter,
        const AppMousePoint& initialPos
    );
    void ctTransformComponent();

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
    std::shared_ptr< BasicTransformComponent > tc_;
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