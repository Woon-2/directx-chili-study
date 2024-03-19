#ifndef __IlluminatedBox
#define __IlluminatedBox

#include "PrimitiveEntity.hpp"
#include "GFX/Scenery/Scene.hpp"
#include "GFX/Scenery/RCDrawComponent.hpp"

#include "GFX/Primitives/Cube.hpp"
#include "App/ChiliWindow.hpp"
#include "GFX/Core/Factory.hpp"

#include "GFX/Scenery/SolidMaterial.hpp"
#include "Image/Surface.hpp"
#include "GFX/PipelineObjects/Texture.hpp"
#include "GFX/PipelineObjects/Sampler.hpp"

class IlluminatedBox {

};

template <>
class gfx::scenery::DrawComponent<IlluminatedBox> : public gfx::scenery::RCDrawCmp {
public:
    struct {} tagVertexBuffer;
    struct {} tagNormalBuffer;
    struct {} tagMaterial;
    struct {} tagTopology;
    struct {} tagTransformCBufV;
    struct {} tagTransformCBufVP;
    struct {} tagViewport;

    class MyVertexBuffer : public gfx::Primitives::Cube::CubeVertexBufferIndependent {
    public:
        MyVertexBuffer(GFXFactory factory)
            : gfx::Primitives::Cube::CubeVertexBufferIndependent(factory) {}

        static constexpr auto size() {
            return gfx::Primitives::Cube::CubeVertexBufferIndependent::size();
        }
    };

    class MyNormalBuffer : public gfx::Primitives::Cube::CubeNormalBufferIndependent {
    public:
        MyNormalBuffer(GFXFactory factory)
            : gfx::Primitives::Cube::CubeNormalBufferIndependent(factory) {}

        static constexpr auto size() {
            return gfx::Primitives::Cube::CubeNormalBufferIndependent::size();
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
    using MyDrawCaller = gfx::po::DrawCaller;

    DrawComponent( gfx::GFXFactory factory, gfx::GFXPipeline pipeline,
        gfx::GFXStorage& storage, const ChiliWindow& wnd
    ) : transformGPUMapperV_(storage),
        transformGPUMapperVP_(storage),
        transformApplyerV_(transformGPUMapperV_),
        transformApplyerVP_(transformGPUMapperVP_),
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        logComponent_(this),
    #endif
        posBuffer_( GFXRes::makeCached<MyVertexBuffer>(storage, tagVertexBuffer, factory) ),
        normalBuffer_( GFXRes::makeCached<MyNormalBuffer>(storage, tagNormalBuffer, factory) ),
        material_( GFXRes::makeCached<MyMaterial>(storage, tagMaterial, factory, storage) ),
        topology_( GFXRes::makeCached<MyTopology>(storage, tagTopology) ),
        viewport_( GFXRes::makeCached<MyViewport>(storage, tagViewport, wnd.client() )),
        transformCBufV_( GFXRes::makeCached<MyTransformCBufV>(storage, tagTransformCBufV, factory) ),
        transformCBufVP_( GFXRes::makeCached<MyTransformCBufVP>(storage, tagTransformCBufVP, factory) ),
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

    void VCALL updateTrans(const gfx::Transform transform) {
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

    void updateMaterial(const gfx::scenery::SolidMaterialDesc& desc) {
        updateDiffuse( dx::XMLoadFloat3A(&desc.diffuse) );
        updateSpecular( dx::XMLoadFloat3(&desc.specular) );
        updateShinyness(desc.shinyness);
        updateAmbient( dx::XMLoadFloat3A(&desc.ambient) );
        updateAmbient( dx::XMLoadFloat3A(&desc.emmisive) );
    }

    void sync(const gfx::scenery::Renderer& renderer) override {
        if ( typeid(renderer) == typeid(gfx::scenery::BPhongRenderer) ) {
            sync( static_cast<const gfx::scenery::BPhongRenderer&>(renderer) );
        }
        else {
            throw GFX_EXCEPT_CUSTOM("DrawComponent tried to synchronize with incompatible renderer.\n");
        }
    }

    void sync(const gfx::scenery::BPhongRenderer& renderer) {
        assert(transformCBufV_.valid());
        transformCBufV_.as<MyTransformCBufV>().setSlot( 0u );

        assert(transformCBufVP_.valid());
        transformCBufVP_.as<MyTransformCBufVP>().setSlot( 1u );

        assert(posBuffer_.valid());
        posBuffer_.as<MyVertexBuffer>().setSlot(
            gfx::scenery::BPhongRenderer::slotPosBuffer()
        );

        assert(normalBuffer_.valid());
        normalBuffer_.as<MyNormalBuffer>().setSlot(
            gfx::scenery::BPhongRenderer::slotNormalBuffer()
        );

        assert(normalBuffer_.valid());
        material_.as<MyMaterial>().setSlot(
            gfx::scenery::BPhongRenderer::slotMaterialCBuffer()
        );

        this->setRODesc( gfx::scenery::RenderObjectDesc{
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

    void sync(const gfx::scenery::CameraVision& vision) {
        transformApplyerV_.setTransform( vision.viewTrans() );
        transformApplyerVP_.setTransform( vision.viewTrans() * vision.projTrans() );
    }

private:
    gfx::scenery::MapTransformGPU transformGPUMapperV_;
    gfx::scenery::MapTransformGPU transformGPUMapperVP_;
    gfx::scenery::ApplyTransform transformApplyerV_;
    gfx::scenery::ApplyTransform transformApplyerVP_;
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
    gfx::scenery::IDrawComponent::LogComponent logComponent_;
#endif
    gfx::GFXRes posBuffer_;
    gfx::GFXRes normalBuffer_;
    gfx::GFXRes material_;
    gfx::GFXRes topology_;
    gfx::GFXRes viewport_;
    gfx::GFXRes transformCBufV_;
    gfx::GFXRes transformCBufVP_;
    std::optional<gfx::scenery::RenderObjectDesc> RODesc_;
    MyDrawCaller drawCaller_;
    gfx::GFXPipeline pipeline_;
    gfx::GFXStorage* pStorage_;
};

template<>
class Entity<IlluminatedBox> : public PrimitiveEntity<IlluminatedBox> {

};

#endif  // __IlluminatedBox