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

    std::ranges::for_each( scene.layers(), [this, &scene](Layer& layer) {
        layer.setup();

        std::ranges::for_each( layer.bindees(), [&](auto* bindee) {
            pipeline_.bind( bindee );
        } );

        std::ranges::for_each( layer.drawCmps(), [&](auto& dc) {
            dc.sync(*this);
            dc.sync(scene.vision());

            auto IDs = std::move(dc.renderObjectDesc().IDs);

            std::ranges::for_each(IDs,
                [&](auto&& id) {
                    pipeline_.bind(pStorage_->get(
                        std::forward<decltype(id)>(id)
                    ).value() );
                }
            );

            pipeline_.drawCall(dc.drawCaller());
        } );
    } );

#ifdef ACTIVATE_RENDERER_LOG
    logComponent().entryStackPop();
#endif
}

SolidRenderer::MyVertexShader::MyVertexShader(GFXFactory factory)
    : VertexShader(factory, inputElemDescs(), csoPath()) {}

std::vector<D3D11_INPUT_ELEMENT_DESC>
SolidRenderer::MyVertexShader::inputElemDescs() const noexcept {
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
SolidRenderer::MyVertexShader::csoPath() const noexcept {
    return compiledShaderPath/L"VertexShaderSolid.cso";
}

SolidRenderer::MyPixelShader::MyPixelShader(GFXFactory factory)
    : PixelShader(factory, csoPath()) {}

std::filesystem::path
SolidRenderer::MyPixelShader::csoPath() const noexcept {
    return compiledShaderPath/L"PixelShaderSolid.cso";
}

const RendererDesc SolidRenderer::rendererDesc() const {
    return RendererDesc{
            .header = {
                .IDVertexShader = vertexShader_.id(),
                .IDPixelShader = pixelShader_.id(),
                .IDType = typeid(*this)
            },
            .IDs = {
                vertexShader_.id(), pixelShader_.id()
            }
        };
}

void SolidRenderer::loadBindables(GFXFactory factory) {
    vertexShader_.config( GFXMappedResource::Type<MyVertexShader>{}, factory );
    vertexShader_.sync(mappedStorage());

    pixelShader_.config( GFXMappedResource::Type<MyPixelShader>{}, factory );
    pixelShader_.sync(mappedStorage());
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
                .IDVertexShader = vertexShader_.id(),
                .IDPixelShader = pixelShader_.id(),
                .IDType = typeid(*this)
            },
            .IDs = {
                vertexShader_.id(), pixelShader_.id()
            }
        };
}

void IndexedRenderer::loadBindables(GFXFactory factory) {
    vertexShader_.config( GFXMappedResource::Type<MyVertexShader>{}, factory );
    vertexShader_.sync(mappedStorage());

    pixelShader_.config( GFXMappedResource::Type<MyPixelShader>{}, factory );
    pixelShader_.sync(mappedStorage());
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
                .IDVertexShader = vertexShader_.id(),
                .IDPixelShader = pixelShader_.id(),
                .IDType = typeid(*this)
            },
            .IDs = {
                vertexShader_.id(), pixelShader_.id()
            }
        };
}

void BlendedRenderer::loadBindables(GFXFactory factory) {
    vertexShader_.config( GFXMappedResource::Type<MyVertexShader>{}, factory );
    vertexShader_.sync(mappedStorage());

    pixelShader_.config( GFXMappedResource::Type<MyPixelShader>{}, factory );
    pixelShader_.sync(mappedStorage());
}

TexturedRenderer::MyVertexShader::MyVertexShader(GFXFactory factory)
    : VertexShader(factory, inputElemDescs(), csoPath()) {}

std::vector<D3D11_INPUT_ELEMENT_DESC>
TexturedRenderer::MyVertexShader::inputElemDescs() const noexcept {
    return std::vector<D3D11_INPUT_ELEMENT_DESC>{
        { .SemanticName = "Position",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        },
        { .SemanticName = "TexCoord",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32_FLOAT,
            .InputSlot = 1u,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
}

std::filesystem::path
TexturedRenderer::MyVertexShader::csoPath() const noexcept {
    return compiledShaderPath/L"VertexShaderTextured.cso";
}

TexturedRenderer::MyPixelShader::MyPixelShader(GFXFactory factory)
    : PixelShader(factory, csoPath()) {}

std::filesystem::path
TexturedRenderer::MyPixelShader::csoPath() const noexcept {
    return compiledShaderPath/L"PixelShaderTextured.cso";
}

const RendererDesc TexturedRenderer::rendererDesc() const {
    return RendererDesc{
            .header = {
                .IDVertexShader = vertexShader_.id(),
                .IDPixelShader = pixelShader_.id(),
                .IDType = typeid(*this)
            },
            .IDs = {
                vertexShader_.id(), pixelShader_.id()
            }
        };
}

void TexturedRenderer::loadBindables(GFXFactory factory) {
    vertexShader_.config( GFXMappedResource::Type<MyVertexShader>{}, factory );
    vertexShader_.sync(mappedStorage());

    pixelShader_.config( GFXMappedResource::Type<MyPixelShader>{}, factory );
    pixelShader_.sync(mappedStorage());
}

BPhongRenderer::MyVertexShader::MyVertexShader(GFXFactory factory)
    : VertexShader(factory, inputElemDescs(), csoPath()) {}

std::vector<D3D11_INPUT_ELEMENT_DESC>
BPhongRenderer::MyVertexShader::inputElemDescs() const noexcept {
    return std::vector<D3D11_INPUT_ELEMENT_DESC>{
        { .SemanticName = "Position",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        },
        { .SemanticName = "Normal",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 1u,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
}

std::filesystem::path
BPhongRenderer::MyVertexShader::csoPath() const noexcept {
    return compiledShaderPath/L"VertexShaderBPhong.cso";
}

BPhongRenderer::MyPixelShader::MyPixelShader(GFXFactory factory)
    : PixelShader(factory, csoPath()) {}

std::filesystem::path
BPhongRenderer::MyPixelShader::csoPath() const noexcept {
    return compiledShaderPath/L"PixelShaderBPhong.cso";
}

const RendererDesc BPhongRenderer::rendererDesc() const {
    return RendererDesc{
            .header = {
                .IDVertexShader = vertexShader_.id(),
                .IDPixelShader = pixelShader_.id(),
                .IDType = typeid(*this)
            },
            .IDs = {
                vertexShader_.id(), pixelShader_.id()
            }
        };
}

void BPhongRenderer::loadBindables(GFXFactory factory) {
    vertexShader_.config( GFXMappedResource::Type<MyVertexShader>{}, factory );
    vertexShader_.sync(mappedStorage());

    pixelShader_.config( GFXMappedResource::Type<MyPixelShader>{}, factory );
    pixelShader_.sync(mappedStorage());
}