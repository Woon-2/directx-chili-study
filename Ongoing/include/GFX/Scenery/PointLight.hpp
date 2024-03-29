#ifndef __PointLight
#define __PointLight

#include "GFX/PipelineObjects/PipelineObject.hpp"
#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/Core/Factory.hpp"
#include "GFX/Core/Storage.hpp"

#include "GFX/Core/Namespaces.hpp"
#include "GFX/Core/Exception.hpp"

#include "Game/CoordSystem.hpp"
#include "Camera.hpp"

#include <ranges>
#include <optional>

namespace gfx {
namespace scenery {

struct BPPointLightDesc {
    dx::XMFLOAT3A pos;
    dx::XMFLOAT3 color;
    float attConst;
    float attLin;
    float attQuad;
};

// for forward declaration of Utilized::BPDynPointLight,
// it is required for specifying Utilized::BPDynPointLight as friend
// in Basic::BPDynPointLight.
namespace Utilized {
    class BPDynPointLight;
}

namespace Basic {

class BPDynPointLight : public po::IPipelineObject{
private:
    using MyPSCBuffer = po::PSCBuffer<BPPointLightDesc>;

public:
    friend class Utilized::BPDynPointLight;

    BPDynPointLight() = default;
    BPDynPointLight(GFXFactory factory);
    BPDynPointLight( GFXFactory factory, 
        const BPPointLightDesc& lightDesc
    );

    void sync(GFXStorage& storage);
    void config(GFXFactory factory);

    const BPPointLightDesc& lightDesc() const noexcept {
        return lightDesc_;
    }

    const dx::XMVECTOR VCALL pos() const noexcept {
        return dx::XMLoadFloat3(&lightDesc_.pos);
    }

    const dx::XMVECTOR VCALL color() const noexcept {
        return dx::XMLoadFloat3(&lightDesc_.color);
    }

    const float attConst() const noexcept {
        return lightDesc_.attConst;
    }

    const float attLin() const noexcept {
        return lightDesc_.attLin;
    }

    const float attQuad() const noexcept {
        return lightDesc_.attQuad;
    }

    void VCALL setPos(dx::FXMVECTOR posVal) {
        dx::XMStoreFloat3A(&lightDesc_.pos, posVal);
        dirty_ = true;
    }

    void VCALL setColor(dx::FXMVECTOR colorVal) {
        dx::XMStoreFloat3(&lightDesc_.color, colorVal);
        dirty_ = true;
    }

    void setAttConst(float attConstVal) {
        lightDesc_.attConst = attConstVal;
        dirty_ = true;
    }

    void setAttLin(float attLinVal) {
        lightDesc_.attLin = attLinVal;
        dirty_ = true;
    }

    void setAttQuad(float attQuadVal) {
        lightDesc_.attQuad = attQuadVal;
        dirty_ = true;
    }

    UINT slot() const {
        return cbuf_.slot();
    }

    void setSlot(UINT val) {
        cbuf_.setSlot(val);
    }

    static const BPPointLightDesc defLightDesc() noexcept;

private:
    void bind(GFXPipeline& pipeline);

    BPPointLightDesc lightDesc_;
    MyPSCBuffer cbuf_;
    bool dirty_;
};

} // namespace Basic

namespace Utilized {

// utilize CoordSystem and CameraVision.
// bind its position as on view space.
// if more modification is needed, adopt decorater pattern.
class BPDynPointLight : public po::IPipelineObject {
public:
    BPDynPointLight() = default;
    BPDynPointLight(GFXFactory factory);
    BPDynPointLight( GFXFactory factory,
        const BPPointLightDesc& lightDesc
    );

    const BPPointLightDesc& lightDesc() const noexcept {
        return base_.lightDesc();
    }

    const dx::XMVECTOR VCALL color() const noexcept {
        return base_.color();
    }

    const float attConst() const noexcept {
        return base_.attConst();
    }

    const float attLin() const noexcept {
        return base_.attLin();
    }

    const float attQuad() const noexcept {
        return base_.attQuad();
    }

    void VCALL setPos(dx::FXMVECTOR posVal) {
        pivot_.setRep(posVal);
    }

    void VCALL setAbsPos(dx::FXMVECTOR posVal) {
        pivot_.setAbs(posVal);
    }

    void VCALL setColor(dx::FXMVECTOR colorVal) {
        base_.setColor(colorVal);
    }

    void setAttConst(float attConstVal) {
        base_.setAttConst(attConstVal);
    }

    void setAttLin(float attLinVal) {
        base_.setAttLin(attLinVal);
    }

    void setAttQuad(float attQuadVal) {
        base_.setAttQuad(attQuadVal);
    }

    UINT slot() const {
        return base_.slot();
    }

    void setSlot(UINT val) {
        base_.setSlot(val);
    }

    CoordSystem& coordSystem() noexcept {
        return coord_;
    }

    const CoordSystem& coordSystem() const noexcept {
        return coord_;
    }

    CoordPt& pos() noexcept {
        return pivot_;
    }

    const CoordPt& pos() const noexcept {
        return pivot_;
    }

    void sync(const CameraVision& vision) {
        pVision_ = &vision;
    }

private:
    void bind(GFXPipeline& pipeline) override;

    CoordSystem coord_;
    Basic::BPDynPointLight base_;
    CoordPt pivot_;
    std::optional<const CameraVision*> pVision_;
};

} // namespace Utilized

} // namespace gfx::scenery
} // namespace gfx

using namespace gfx::scenery::Utilized;

#include "Game/Entity.hpp"
#include "Game/Chrono.hpp"

#include "Scene.hpp"
#include "Renderer.hpp"
#include "RCDrawComponent.hpp"
#include "TransformDrawContexts.hpp"

#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/PipelineObjects/Viewport.hpp"
#include "GFX/PipelineObjects/Topology.hpp"
#include "GFX/PipelineObjects/DrawCaller.hpp"
#include "GFX/Primitives/Sphere.hpp"

#include <tuple>

namespace gfx {
namespace scenery {

class Luminance : public IEntity {
public:
    friend class Loader<Luminance>;

    Luminance(GFXFactory factory, GFXStorage& storage);

    void sync(const Renderer& renderer);
    void sync(const BPhongRenderer& renderer);

    GFXRes& res() noexcept {
        return res_;
    }

    const GFXRes& res() const noexcept {
        return res_;
    }

    void update(milliseconds elasped) override {}

    Loader<Luminance> loader() noexcept;

private:
    GFXRes res_;
};

template <>
class Loader<Luminance> {
public:
    Loader(Luminance& entity)
        : entity_(entity) {}

    void loadAt(LSceneAdapter scene) {
        scene.addLight(entity_.res());
        entity_.res().as<BPDynPointLight>().sync(scene.vision());
    }
    void loadAt(const CoordSystem& coord) {
        entity_.res().as<BPDynPointLight>()
            .coordSystem().setParent(coord);
    }

private:
    Luminance& entity_;
};

inline Loader<Luminance> Luminance::loader() noexcept {
    return Loader<Luminance>(*this);
}

class LightViz : public IEntity {
public:
    friend class Loader<LightViz>;

    LightViz( GFXFactory factory, GFXStorage& storage,
        const Win32::Client& client
    );
    LightViz( const Luminance& base, GFXFactory factory,
        GFXStorage& storage, const Win32::Client& client
    );

    void setBase(const Luminance& base) {
        base_ = &base;
    }

    void update(milliseconds elasped) override {
        auto& res = base_->res().as<BPDynPointLight>();
        dc_.updateTrans( res.coordSystem().total().get()
            * dx::XMMatrixTranslationFromVector( res.pos().rep() )
        );
        dc_.updateColor( res.color() );
    }

    Loader<LightViz> loader() noexcept;

private:
    class DrawComponentLViz : public RCDrawCmp {
    public:
        struct {} tagVertexBuffer;
        struct {} tagIndexBuffer;
        struct {} tagColorCBuf;
        struct {} tagDynColorCBuf;
        struct {} tagTransformCBuf;
        struct {} tagViewport;
        struct {} tagTopology;

        class MyVertexBuffer;
        class MyIndexBuffer;
        class MyColorCBuf;
        class MyDynColorCBuf;
        class MyTransformCBuf;
        class MyViewport;
        class MyTopology;
        using MyDrawCaller = po::DrawCallerIndexed;

        DrawComponentLViz( GFXFactory factory, GFXStorage& storage,
            const Win32::Client& client
        );
        DrawComponentLViz(DrawComponentLViz&& other) noexcept;
        DrawComponentLViz& operator=(DrawComponentLViz&& other) noexcept;

        void VCALL updateTrans(const Transform transform);
        void VCALL updateColor(dx::FXMVECTOR color);

        void sync(const Renderer& renderer) override;
        void sync(const SolidRenderer& renderer);
        void sync(const CameraVision& vision) override;

        auto reflect() noexcept {
            return std::tie( transformGPUMapper_, transformApplyer_,
                logComponent_, vBuf_, iBuf_, colorCBuf_, transCBuf_,
                viewport_, topology_
            );
        }

        auto reflect() const noexcept {
            return std::tie( transformGPUMapper_, transformApplyer_,
                logComponent_, vBuf_, iBuf_, colorCBuf_, transCBuf_,
                viewport_, topology_
            );
        }

        void swap(DrawComponentLViz& rhs);

    private:
        MapTransformGPU transformGPUMapper_;
        ApplyTransform transformApplyer_;
    #ifdef ACTIVATE_DRAWCOMPONENT_LOG
        RCDrawCmp::LogComponent logComponent_;
    #endif
        GFXRes vBuf_;
        GFXRes iBuf_;
        GFXRes colorCBuf_;
        GFXRes transCBuf_;
        GFXRes viewport_;
        GFXRes topology_;
    };  // class DrawComponentLViz

    DrawComponentLViz& dc() noexcept {
        return dc_;
    }

    const DrawComponentLViz& dc() const noexcept {
        return dc_;
    }

    DrawComponentLViz dc_;
    const Luminance* base_;
};

template <>
class Loader<LightViz> {
public:
    Loader(LightViz& entity)
        : entity_(entity) {}

    void loadAt(Layer& layer) {
        layer.addDrawCmp(&entity_.dc());
    }

    void loadAt(Scene& scene, std::size_t layerIdx) {
        loadAt(scene.layer(layerIdx));
    }

private:
    LightViz& entity_;
};

inline Loader<LightViz> LightViz::loader() noexcept {
    return Loader<LightViz>(*this);
}

class LightEntity : public IEntity {
public:
    void update(milliseconds elapsed) override {
        if (luminance_.has_value()) [[likely]] {
            luminance_.value().update(elapsed);
        }
        if (viz_.has_value()) {
            viz_.value().update(elapsed);
        }
    }

    void ctLuminance(GFXFactory factory, GFXStorage& storage) {
        luminance_ = Luminance(std::move(factory), storage);
        if (viz_.has_value()) {
            viz_.value().setBase(luminance_.value());
        }
    }

    void ctViz( GFXFactory factory, GFXStorage& storage,
        const Win32::Client& client
    ) {
        if (luminance_.has_value()) {
            viz_ = LightViz( luminance_.value(),
                std::move(factory), storage, client
            );
        }
        else {
            viz_ = LightViz(std::move(factory), storage, client);
        }
    }

    Luminance& luminance() noexcept {
        assert(hasLuminance());
        return luminance_.value();
    }

    const Luminance& luminance() const noexcept {
        assert(hasLuminance());
        return luminance_.value();
    }

    bool hasLuminance() const noexcept {
        return luminance_.has_value();
    }

    LightViz& viz() noexcept {
        assert(hasViz());
        return viz_.value();
    }

    const LightViz& viz() const noexcept {
        assert(hasViz());
        return viz_.value();
    }

    bool hasViz() const noexcept {
        return viz_.has_value();
    }

private:
    std::optional<Luminance> luminance_;
    std::optional<LightViz> viz_;
};

}  // namespace gfx::scenery
}  // namespace gfx

#endif  // __PointLight