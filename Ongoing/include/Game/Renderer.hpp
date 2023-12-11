#ifndef __Renderer
#define __Renderer

#include "Scene.hpp"
#include "RendererDesc.hpp"

#include "GFX/Core/Pipeline.hpp"
#include "GFX/PipelineObjects/Shader.hpp"
#include "GFX/Core/GraphicsStorage.hpp"

#include <d3d11.h>

#include <vector>
#include <filesystem>

class Renderer {
public:
    Renderer() = default;
    Renderer(GFXPipeline pipeline)
        : pipeline_( std::move(pipeline) ) {}

    virtual ~Renderer() {}

    void linkPipeline(GFXPipeline& pipeline) {
        pipeline_ = pipeline;
    }

    void linkPipeline(GFXPipeline&& pipeline) {
        pipeline_ = std::move(pipeline);
    }

    void render(Scene& scene);

    void sync(GFXStorage& storage, GFXFactory factory) {
        pStorage_ = &storage;
        loadBindables(std::move(factory));
    }

    GFXStorage& mappedStorage() noexcept {
        return *pStorage_;
    }

    const GFXStorage& mappedStorage() const noexcept {
        return *pStorage_;
    }

private:
    virtual const RendererDesc rendererDesc() const = 0;
    virtual void loadBindables(GFXFactory factory) = 0;

    GFXPipeline pipeline_;
    GFXStorage* pStorage_;
};

class IndexedRenderer : public Renderer {
public:
    class MyVertexShader : public VertexShader {
    public:
        MyVertexShader(GFXFactory factory);

    private:
        std::vector<D3D11_INPUT_ELEMENT_DESC> inputElemDescs() const noexcept;
        std::filesystem::path csoPath() const noexcept;
    };

    class MyPixelShader : public PixelShader {
    public:
        MyPixelShader(GFXFactory factory);

    private:
        std::filesystem::path csoPath() const noexcept;
    };

    IndexedRenderer() = default;
    IndexedRenderer(GFXPipeline pipeline)
        : Renderer(std::move(pipeline)) {}

private:
    const RendererDesc rendererDesc() const override;

    void loadBindables(GFXFactory factory) override {
        IDVertexShader_ = mappedStorage().cache<MyVertexShader>(factory);
        IDPixelShader_ = mappedStorage().cache<MyPixelShader>(factory);
    }

    GFXStorage* pStorage_;
    GFXStorage::ID IDVertexShader_;
    GFXStorage::ID IDPixelShader_;
};

#endif  // __Renderer