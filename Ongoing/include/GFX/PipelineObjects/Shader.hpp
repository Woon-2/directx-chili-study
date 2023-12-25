#ifndef __Shader
#define __Shader

#include "Bindable.hpp"
#include "GFX/Core/GFXFactory.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

#include <ranges>
#include <filesystem>

class VertexShaderBinder : public BinderInterface<VertexShaderBinder> {
public:
    friend class BinderInterface<VertexShaderBinder>; 

private:
    void doBind(GFXPipeline& pipeline, ID3D11VertexShader* pShader,
        ID3D11InputLayout* pIA
    );
};

class VertexShader : public IBindable,
    public LocalRebindInterface<VertexShader> {
public:
    friend class LocalRebindInterface<VertexShader>;

    template <std::ranges::contiguous_range InputElemDescArray>
    VertexShader( GFXFactory factory,
        const InputElemDescArray& ieDescs,
        const std::filesystem::path& path
    #ifdef ACTIVATE_BINDABLE_LOG
        , bool enableLogOnCreation = true
    #endif
    ) :
    #ifdef ACTIVATE_BINDABLE_LOG
        logComponent_( this, GFXCMDSourceCategory("VertexShader") ),
    #endif
        byteCode_(), pVertexShader_(), pInputLayout_(), binder_()
    {
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

    #ifdef ACTIVATE_BINDABLE_LOG
        if (enableLogOnCreation) {
            logComponent_.enableLog();
        }
        logComponent_.logCreate();
    #endif
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
    void bind(GFXPipeline& pipeline) override final;

#ifdef ACTIVATE_BINDABLE_LOG
    IBindable::LogComponent logComponent_;
#endif
    wrl::ComPtr<ID3DBlob> byteCode_;
    wrl::ComPtr<ID3D11VertexShader> pVertexShader_;
    wrl::ComPtr<ID3D11InputLayout> pInputLayout_;
    VertexShaderBinder binder_;
};

class PixelShaderBinder : public BinderInterface<PixelShaderBinder> {
public:
    friend class BinderInterface<PixelShaderBinder>;

private:
    void doBind(GFXPipeline& pipeline, ID3D11PixelShader* pShader);
};

class PixelShader : public IBindable,
    public LocalRebindInterface<PixelShader> {
public:
    friend class LocalRebindInterface<PixelShader>;

    PixelShader( GFXFactory factory,
        const std::filesystem::path& path
    #ifdef ACTIVATE_BINDABLE_LOG
        , bool enableLogOnCreation = true
    #endif
    );

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
    void bind(GFXPipeline& pipeline) override final;

#ifdef ACTIVATE_BINDABLE_LOG
    IBindable::LogComponent logComponent_;
#endif
    wrl::ComPtr<ID3DBlob> byteCode_;
    wrl::ComPtr<ID3D11PixelShader> pPixelShader_;
    PixelShaderBinder binder_;
};

#endif  // __Shader