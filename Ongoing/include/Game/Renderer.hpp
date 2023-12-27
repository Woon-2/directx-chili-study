#ifndef __Renderer
#define __Renderer

#define ACTIVATE_RENDERER_LOG

#include "Scene.hpp"
#include "RendererDesc.hpp"

#include "GFX/Core/Pipeline.hpp"
#include "GFX/PipelineObjects/Shader.hpp"
#include "GFX/Core/GraphicsStorage.hpp"

#include <d3d11.h>

#include <vector>
#include <filesystem>

class Renderer {
#ifdef ACTIVATE_RENDERER_LOG
private:
    class LogComponent {
    public:
        LogComponent() noexcept
            : logSrc_(nullptr), bLogEnabled_(false) {}
        
        LogComponent(const Renderer* parent) noexcept
            : logSrc_(parent), bLogEnabled_(false) {}

        void enableLog() noexcept {
            bLogEnabled_ = true;
        }

        void disableLog() noexcept {
            bLogEnabled_ = false;
        }

        bool logEnabled() const noexcept {
            return bLogEnabled_;
        }

        void entryStackPush();
        void entryStackPop() noexcept;

    private:
        const Renderer* logSrc_;
        bool bLogEnabled_;
    };
#endif  // ACTIVATE_RENDERER_LOG
public:
    Renderer()
        : pipeline_(), pStorage_(nullptr)
    #ifdef ACTIVATE_RENDERER_LOG
        ,logComponent_(this)
    #endif
        {}

    Renderer(GFXPipeline pipeline)
        : pipeline_( std::move(pipeline) ), pStorage_(nullptr)
    #ifdef ACTIVATE_RENDERER_LOG
        ,logComponent_(this)
    #endif
        {}

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

#ifdef ACTIVATE_RENDERER_LOG
    void enableLog() noexcept {
        logComponent().enableLog();
    }

    void disableLog() noexcept {
        logComponent().disableLog();
    }

    bool logEnabled() const noexcept {
        return logComponent().logEnabled();
    }

    static constexpr const GFXCMDSourceCategory
        logCategory() noexcept {
        return GFXCMDSourceCategory("Renderer");
    }
#endif

protected:
#ifdef ACTIVATE_RENDERER_LOG
    LogComponent& logComponent() noexcept {
        return logComponent_;
    }

    const LogComponent& logComponent() const noexcept {
        return logComponent_;
    }
#endif  // ACTIVATE_RENDERER_LOG
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
#ifdef ACTIVATE_RENDERER_LOG
    LogComponent logComponent_;
#endif // ACTIVATE_RENDERER_LOG
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

    static consteval UINT slotPosBuffer() {
        return 0u;
    }

    IndexedRenderer() = default;
    IndexedRenderer(GFXPipeline pipeline)
        : Renderer(std::move(pipeline)) {}

private:
    const RendererDesc rendererDesc() const override;

    void loadBindables(GFXFactory factory) override;

    GFXStorage* pStorage_;
    GFXStorage::ID IDVertexShader_;
    GFXStorage::ID IDPixelShader_;
};

class BlendedRenderer : public Renderer {
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

    BlendedRenderer() = default;
    BlendedRenderer(GFXPipeline pipeline)
        : Renderer(std::move(pipeline)) {}

    static consteval UINT slotPosBuffer() {
        return 0u;
    }

    static consteval UINT slotColorBuffer() {
        return 1u;
    }

private:
    const RendererDesc rendererDesc() const override;
    void loadBindables(GFXFactory factory) override;

    GFXStorage* pStorage_;
    GFXStorage::ID IDVertexShader_;
    GFXStorage::ID IDPixelShader_;
};

class TexturedRenderer : public Renderer {
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

    TexturedRenderer() = default;
    TexturedRenderer(GFXPipeline pipeline)
        : Renderer(std::move(pipeline)) {}

    static consteval UINT slotPosBuffer() {
        return 0u;
    }

    static consteval UINT slotTexBuffer() {
        return 1u;
    }

    static consteval UINT slotTexture() {
        return 0u;
    }

    static consteval UINT slotSamplerState() {
        return 0u;
    }

private:
    const RendererDesc rendererDesc() const override;
    void loadBindables(GFXFactory factory) override;

    GFXStorage* pStorage_;
    GFXStorage::ID IDVertexShader_;
    GFXStorage::ID IDPixelShader_;
};

#endif  // __Renderer