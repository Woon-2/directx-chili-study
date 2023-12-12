#include "Game/Renderer.hpp"
#include "Game/RenderObjectDesc.hpp"

#include "GFX/PipelineObjects/Bindable.hpp"

#include <ranges>
#include <algorithm>

#include "ShaderPath.h"

void Renderer::render(Scene& scene) {
    std::ranges::for_each( std::move(rendererDesc().IDs),
        [this](auto&& id) {
            pipeline_.bind( mappedStorage().get(
                std::forward<decltype(id)>(id)
            ).value() );
        }
    );

    std::ranges::for_each( scene.drawComponents(),
        [this, &scene](const auto& dc) mutable {
            auto IDs = std::move(dc.renderObjectDesc().IDs);

            std::ranges::for_each(IDs,
                [&](auto&& id) {
                    pipeline_.bind(scene.storage().get(
                        std::forward<decltype(id)>(id)
                    ).value() );
                }
            );

        pipeline_.drawCall(dc.drawContext());
        }
    );
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
    return compiledShaderPath/L"VertexShader.cso";
}

IndexedRenderer::MyPixelShader::MyPixelShader(GFXFactory factory)
    : PixelShader(factory, csoPath()) {}

std::filesystem::path
IndexedRenderer::MyPixelShader::csoPath() const noexcept {
    return compiledShaderPath/L"PixelShader.cso";
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