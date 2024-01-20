#ifndef __IlluminatedBox
#define __IlluminatedBox

#include "PrimitiveEntity.hpp"
#include "Scene.hpp"
#include "RCDrawComponent.hpp"

#include "GFX/Primitives/Cube.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/GFXFactory.hpp"

#include "SolidMaterial.hpp"
#include "Image/Surface.hpp"
#include "GFX/PipelineObjects/Texture.hpp"
#include "GFX/PipelineObjects/Sampler.hpp"

class IlluminatedBox {

};

template <>
class DrawComponent<IlluminatedBox> : public RCDrawCmp {
public:
    class MyVertexBuffer : public Primitives::Cube::CubeVertexBufferIndependent {
    public:
        MyVertexBuffer(GFXFactory factory)
            : Primitives::Cube::CubeVertexBufferIndependent(factory) {}

        static constexpr auto size() {
            return Primitives::Cube::CubeVertexBufferIndependent::size();
        }
    };

    class MyNormalBuffer : public Primitives::Cube::CubeNormalBufferIndependent {
    public:
        MyNormalBuffer(GFXFactory factory)
            : Primitives::Cube::CubeNormalBufferIndependent(factory) {}

        static constexpr auto size() {
            return Primitives::Cube::CubeNormalBufferIndependent::size();
        }
    };

    using MyMaterial = SolidMaterial;

    using MyTopology = PETopology;

    class MyTransformCBufV : public PETransformCBuf {
    public:
        MyTransformCBufV() = default;
        MyTransformCBufV(GFXFactory factory)
            : PETransformCBuf(std::move(factory)) {}
    };

    class MyTransformCBufVP : public PETransformCBuf {
    public:
        MyTransformCBufVP() = default;
        MyTransformCBufVP(GFXFactory factory)
            : PETransformCBuf(std::move(factory)) {}
    };

    using MyViewport = PEViewport;
    using MyDrawCaller = DrawCaller;

    DrawComponent( GFXFactory factory, GFXPipeline pipeline,
        GFXStorage& storage, const ChiliWindow& wnd
    ) : transformGPUMapperV_(storage),
        transformGPUMapperVP_(storage),
        transformApplyerV_(transformGPUMapperV_),
        transformApplyerVP_(transformGPUMapperVP_),
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        posBuffer_( GFXMappedResource::Type<MyVertexBuffer>{},
            typeid(MyVertexBuffer), storage, factory
        ),
        normalBuffer_( GFXMappedResource::Type<MyNormalBuffer>{},
            typeid(MyNormalBuffer), storage, factory
        ),
        material_( GFXMappedResource::Type<MyMaterial>{},
            typeid(MyMaterial), storage, factory, storage
        ),
        topology_( GFXMappedResource::Type<MyTopology>{},
            typeid(MyTopology), storage
        ),
        viewport_( GFXMappedResource::Type<MyViewport>{},
            typeid(MyViewport), storage,  wnd.client()
        ),
        transformCBufV_( GFXMappedResource::Type<MyTransformCBufV>{},
            typeid(MyTransformCBufV), storage, factory
        ),
        transformCBufVP_( GFXMappedResource::Type<MyTransformCBufVP>{},
            typeid(MyTransformCBufVP), storage, factory
        ),
        pipeline_(pipeline), pStorage_(&storage) {

        transformGPUMapperV_.setTCBufID(transformCBufV_.id());
        transformGPUMapperVP_.setTCBufID(transformCBufVP_.id());

        this->setDrawCaller( std::make_unique<MyDrawCaller>(
            static_cast<UINT>( MyVertexBuffer::size() ), 0
        ) );

        this->drawCaller().addDrawContext(&transformApplyerV_);
        this->drawCaller().addDrawContext(&transformGPUMapperV_);

        this->drawCaller().addDrawContext(&transformApplyerVP_);
        this->drawCaller().addDrawContext(&transformGPUMapperVP_);
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_.entryStackPop();
    #endif
    }

    void VCALL updateTrans(const Transform transform) {
        transformGPUMapperV_.update(transform);
        transformGPUMapperVP_.update(transform);
    }

    void VCALL updateDiffuse(dx::FXMVECTOR color) {
        material_.as<MyMaterial>().setDiffuse(color);
    }

    void VCALL updateSpecular(dx::FXMVECTOR color) {
        material_.as<MyMaterial>().setSpecular(color);
    }

    void updateShinyness(float shinyness) {
        material_.as<MyMaterial>().setShinyness(shinyness);
    }

    void VCALL updateAmbient(dx::FXMVECTOR color) {
        material_.as<MyMaterial>().setAmbient(color);
    }

    void VCALL updateEmmisive(dx::FXMVECTOR color) {
        material_.as<MyMaterial>().setEmmisive(color);
    }

    void updateMaterial(const SolidMaterialDesc& desc) {
        updateDiffuse( dx::XMLoadFloat3A(&desc.diffuse) );
        updateSpecular( dx::XMLoadFloat3(&desc.specular) );
        updateShinyness(desc.shinyness);
        updateAmbient( dx::XMLoadFloat3A(&desc.ambient) );
        updateAmbient( dx::XMLoadFloat3A(&desc.emmisive) );
    }

    void sync(const Renderer& renderer) override {
        if ( typeid(renderer) == typeid(BPhongRenderer) ) {
            sync( static_cast<const BPhongRenderer&>(renderer) );
        }
        else {
            throw GFX_EXCEPT_CUSTOM("DrawComponent tried to synchronize with incompatible renderer.\n");
        }
    }

    void sync(const BPhongRenderer& renderer) {
        assert(transformCBufV_.valid());
        transformCBufV_.as<MyTransformCBufV>().setSlot( 0u );

        assert(transformCBufVP_.valid());
        transformCBufVP_.as<MyTransformCBufVP>().setSlot( 1u );

        assert(posBuffer_.valid());
        posBuffer_.as<MyVertexBuffer>().setSlot(
            BPhongRenderer::slotPosBuffer()
        );

        assert(normalBuffer_.valid());
        normalBuffer_.as<MyNormalBuffer>().setSlot(
            BPhongRenderer::slotNormalBuffer()
        );

        assert(normalBuffer_.valid());
        material_.as<MyMaterial>().setSlot(
            BPhongRenderer::slotMaterialCBuffer()
        );

        this->setRODesc( RenderObjectDesc{
            .header = {
                .IDBuffer = posBuffer_.id(),
                .IDType = typeid(IlluminatedBox)
            },
            .IDs = {
                posBuffer_.id(), normalBuffer_.id(), material_.id(),
                topology_.id(), viewport_.id(), transformCBufV_.id(),
                transformCBufVP_.id()
            }
        } );
    }

    void sync(const CameraVision& vision) {
        transformApplyerV_.setTransform( vision.viewTrans() );
        transformApplyerVP_.setTransform( vision.viewTrans() * vision.projTrans() );
    }

private:
    MapTransformGPU transformGPUMapperV_;
    MapTransformGPU transformGPUMapperVP_;
    ApplyTransform transformApplyerV_;
    ApplyTransform transformApplyerVP_;
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
    IDrawComponent::LogComponent logComponent_;
#endif
    GFXMappedResource posBuffer_;
    GFXMappedResource normalBuffer_;
    GFXMappedResource material_;
    GFXMappedResource topology_;
    GFXMappedResource viewport_;
    GFXMappedResource transformCBufV_;
    GFXMappedResource transformCBufVP_;
    std::optional<RenderObjectDesc> RODesc_;
    MyDrawCaller drawCaller_;
    GFXPipeline pipeline_;
    GFXStorage* pStorage_;
};

template<>
class Entity<IlluminatedBox> : public PrimitiveEntity<IlluminatedBox> {

};

#endif  // __IlluminatedBox