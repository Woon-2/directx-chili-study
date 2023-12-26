#include "Game/Renderer.hpp"
#include "Game/RenderObjectDesc.hpp"

#include "GFX/PipelineObjects/Bindable.hpp"

#include "Game/GFXCMDLogger.hpp"

#include <ranges>
#include <algorithm>

#include "ShaderPath.h"

#ifdef ACTIVATE_RENDERER_LOG
void Renderer::LogComponent::entryStackPush() {
    GFXCMDLOG.entryStackPush( GFXCMDSource{
        .category = logCategory(),
        .pSource = logSrc_
    } );
}

void Renderer::LogComponent::entryStackPop() noexcept {
    GFXCMDLOG.entryStackPop();
}
#endif  // ACTIVATE_RENDERER_LOG

void Renderer::render(Scene& scene) {
#ifdef ACTIVATE_RENDERER_LOG
    logComponent().entryStackPush();
#endif

    std::ranges::for_each( std::move(rendererDesc().IDs),
        [this](auto&& id) {
            pipeline_.bind( mappedStorage().get(
                std::forward<decltype(id)>(id)
            ).value() );
        }
    );

    std::ranges::for_each( scene.drawComponents(),
        [this, &scene](auto& dc) mutable {
            dc.sync(*this);
            auto IDs = std::move(dc.renderObjectDesc().IDs);

            std::ranges::for_each(IDs,
                [&](auto&& id) {
                    pipeline_.bind(pStorage_->get(
                        std::forward<decltype(id)>(id)
                    ).value() );
                }
            );

            pipeline_.drawCall(dc.drawContext());
        }
    );

#ifdef ACTIVATE_RENDERER_LOG
    logComponent().entryStackPop();
#endif
}

IndexedRenderer::MyVertexShader::MyVertexShader(GFXFactory factory)
    : VertexShader(factory, inputElemDescs(), csoPath()) {}

std::vector<D3D11_INPUT_ELEMENT_DESC>
IndexedRenderer::MyVertexShader::inputElemDescs() const noexcept {
    return std::vector<D3D11_INPUT_ELEMENT_DESC>{
        { .SemanticName = "Position",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
}

std::filesystem::path
IndexedRenderer::MyVertexShader::csoPath() const noexcept {
    return compiledShaderPath/L"VertexShaderIndexed.cso";
}

IndexedRenderer::MyPixelShader::MyPixelShader(GFXFactory factory)
    : PixelShader(factory, csoPath()) {}

std::filesystem::path
IndexedRenderer::MyPixelShader::csoPath() const noexcept {
    return compiledShaderPath/L"PixelShaderIndexed.cso";
}

const RendererDesc IndexedRenderer::rendererDesc() const {
    return RendererDesc{
            .header = {
                .IDVertexShader = IDVertexShader_,
                .IDPixelShader = IDPixelShader_,
                .IDType = typeid(*this)
            },
            .IDs = {
                IDVertexShader_, IDPixelShader_
            }
        };
}

void IndexedRenderer::loadBindables(GFXFactory factory) {
    IDVertexShader_ = mappedStorage().cache<MyVertexShader>(factory);
    IDPixelShader_ = mappedStorage().cache<MyPixelShader>(factory);
}

BlendedRenderer::MyVertexShader::MyVertexShader(GFXFactory factory)
    : VertexShader(factory, inputElemDescs(), csoPath()) {}

std::vector<D3D11_INPUT_ELEMENT_DESC>
BlendedRenderer::MyVertexShader::inputElemDescs() const noexcept {
    return std::vector<D3D11_INPUT_ELEMENT_DESC>{
        { .SemanticName = "Position",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        },
        { .SemanticName = "Color",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 1,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
}

std::filesystem::path
BlendedRenderer::MyVertexShader::csoPath() const noexcept {
    return compiledShaderPath/L"VertexShaderBlended.cso";
}

BlendedRenderer::MyPixelShader::MyPixelShader(GFXFactory factory)
    : PixelShader(factory, csoPath()) {}

std::filesystem::path
BlendedRenderer::MyPixelShader::csoPath() const noexcept {
    return compiledShaderPath/L"PixelShaderBlended.cso";
}

const RendererDesc BlendedRenderer::rendererDesc() const {
    return RendererDesc{
            .header = {
                .IDVertexShader = IDVertexShader_,
                .IDPixelShader = IDPixelShader_,
                .IDType = typeid(*this)
            },
            .IDs = {
                IDVertexShader_, IDPixelShader_
            }
        };
}

void BlendedRenderer::loadBindables(GFXFactory factory) {
    IDVertexShader_ = mappedStorage().cache<MyVertexShader>(factory);
    IDPixelShader_ = mappedStorage().cache<MyPixelShader>(factory);
}