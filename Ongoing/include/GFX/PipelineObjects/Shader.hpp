#ifndef __Shader
#define __Shader

#include "Bindable.hpp"

#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/Pipeline.hpp"
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>

#include <ranges>
#include <filesystem>

class VertexShader : public IBindable {
public:
    template <std::ranges::contiguous_range InputElemDescArray>
    VertexShader( GFXFactory factory,
        const InputElemDescArray& ieDescs,
        const std::filesystem::path& path
    ) {
        GFX_THROW_FAILED(
            D3DReadFileToBlob(path.c_str(), &byteCode_)
        );
        GFX_THROW_FAILED(
            factory.device()->CreateVertexShader( byteCode(),
                byteCodeLength(), nullptr, &pVertexShader_
            )
        );
        GFX_THROW_FAILED(
            factory.device()->CreateInputLayout(
                std::data(ieDescs),
                static_cast<UINT>( std::size(ieDescs) ),
                byteCode(),
                byteCodeLength(),
                &pInputLayout_
            )
        );
    }

    const wrl::ComPtr<ID3DBlob> byteCodeBlob() const noexcept {
        return byteCode_;
    }

    const void* byteCode() const noexcept {
        return byteCodeBlob()->GetBufferPointer();
    }

    const SIZE_T byteCodeLength() const noexcept {
        return byteCodeBlob()->GetBufferSize();
    }

private:
    void bind(GFXPipeline& pipeline) override {
        GFX_THROW_FAILED_VOID(
            pipeline.context()->VSSetShader(
                pVertexShader_.Get(), nullptr, 0
            )
        );
        GFX_THROW_FAILED_VOID(
            pipeline.context()->IASetInputLayout(
                pInputLayout_.Get()
            )
        );
    }

    wrl::ComPtr<ID3DBlob> byteCode_;
    wrl::ComPtr<ID3D11VertexShader> pVertexShader_;
    wrl::ComPtr<ID3D11InputLayout> pInputLayout_;
};

class PixelShader : public IBindable {
public:
    PixelShader( GFXFactory factory,
        const std::filesystem::path& path
    ) {
        GFX_THROW_FAILED(
            D3DReadFileToBlob(path.c_str(), &byteCode_)
        );
        GFX_THROW_FAILED(
            factory.device()->CreatePixelShader( byteCode(),
                byteCodeLength(), nullptr, &pPixelShader_
            )
        );
    }

    const wrl::ComPtr<ID3DBlob> byteCodeBlob() const noexcept {
        return byteCode_;
    }

    const void* byteCode() const noexcept {
        return byteCodeBlob()->GetBufferPointer();
    }

    const SIZE_T byteCodeLength() const noexcept {
        return byteCodeBlob()->GetBufferSize();
    }
private:
    void bind(GFXPipeline& pipeline) override {
        GFX_THROW_FAILED_VOID(
            pipeline.context()->PSSetShader(
                pPixelShader_.Get(), nullptr, 0u
            )
        );
    }

    wrl::ComPtr<ID3DBlob> byteCode_;
    wrl::ComPtr<ID3D11PixelShader> pPixelShader_;
};

#endif  // __Shader