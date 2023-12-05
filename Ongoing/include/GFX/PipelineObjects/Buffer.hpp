#ifndef __Buffer
#define __Buffer

#include "Bindable.hpp"
#include "GFX/Core/Pipeline.hpp"
#include "GFX/Core/GFXFactory.hpp"

#include "App/ChiliWindow.hpp"
#include <d3d11.h>
#include <DirectXMath.h>
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

#include <ranges>
#include <type_traits>
#include <algorithm>
#include <concepts>
#include <functional>

class Buffer : public IBindable {
public:
    Buffer( GFXFactory factory,
        const D3D11_BUFFER_DESC& bufferDesc,
        const D3D11_SUBRESOURCE_DATA& subresourceData
    ) : desc_(bufferDesc), data_() {
        GFX_THROW_FAILED(
            factory.device()->CreateBuffer(&bufferDesc,
                &subresourceData, &data_
            )
        );
    }

    // https://learn.microsoft.com/en-us/windows/win32/direct3d11/how-to--use-dynamic-resources
    // there's a room for enhancing performance
    // by using D3D11_MAP_WRITE_NO_OVERWRITE and D3D11_MAP_WRITE_DISCARD interleaved.
    template <class BufferGetter>
        requires std::convertible_to< std::invoke_result_t<BufferGetter>, void* >
            || std::convertible_to< std::invoke_result_t<BufferGetter>, const void* >
    void dynamicUpdate( GFXPipeline& pipeline, BufferGetter&& getter ) {
        // is dynamic usage check to be added
        auto mapped = D3D11_MAPPED_SUBRESOURCE{};
        std::fill_n( reinterpret_cast<char*>(&mapped), sizeof(mapped), 0 );

        GFX_THROW_FAILED(
            pipeline.context()->Map( data().Get(), 0u,
                D3D11_MAP_WRITE_DISCARD, 0, &mapped
            )
        );

        const void* updated = std::invoke( std::forward<BufferGetter>(getter) );

        std::copy_n( static_cast<const char*>(updated),
            desc_.ByteWidth, static_cast<char*>(mapped.pData)
        );

        GFX_THROW_FAILED_VOID(
            pipeline.context()->Unmap( data().Get(), 0 )
        );
    }

    const wrl::ComPtr<ID3D11Buffer> data() const {
        return data_;
    }

    wrl::ComPtr<ID3D11Buffer> data() {
        return data_;
    }

private:
    virtual void bind(GFXPipeline& pipeline) = 0;

    D3D11_BUFFER_DESC desc_;
    wrl::ComPtr<ID3D11Buffer> data_;
};


template <class VertexT>
class VertexBuffer : public Buffer {
public:
    using MyVertex = VertexT;

    template <std::ranges::contiguous_range R>
    VertexBuffer( GFXFactory factory,
        R&& range
    ) : Buffer( std::move(factory),
            D3D11_BUFFER_DESC{
                .ByteWidth = static_cast<UINT>(
                    std::size(range) * sizeof(MyVertex)
                ),
                .Usage = D3D11_USAGE_DEFAULT,
                .BindFlags = D3D11_BIND_VERTEX_BUFFER,
                .CPUAccessFlags = 0,
                .MiscFlags = 0,
                .StructureByteStride = sizeof(MyVertex)
            },
            D3D11_SUBRESOURCE_DATA{
                .pSysMem = std::data(range),
                .SysMemPitch = 0,
                .SysMemSlicePitch = 0
            }
        ) {}

private:
    void bind(GFXPipeline& pipeline) override {
        const auto stride = static_cast<UINT>( sizeof(MyVertex) );
        const auto offset = static_cast<UINT>( 0u );

        GFX_THROW_FAILED_VOID(
            pipeline.context()->IASetVertexBuffers(
                0u, 1u, data().GetAddressOf(),
                &stride, &offset
            )
        );
    } 
};

template <class IndexT>
class IndexBuffer : public Buffer {
public:
    using MyIndex = IndexT;

    template <std::ranges::contiguous_range R>
    IndexBuffer( GFXFactory factory,
        R&& range
    ) : Buffer( std::move(factory),
        D3D11_BUFFER_DESC{
            .ByteWidth = static_cast<UINT>(
                std::size(range) * sizeof(MyIndex)
            ),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
            .CPUAccessFlags = 0u,
            .MiscFlags = 0u,
            .StructureByteStride = sizeof(MyIndex)
        },
        D3D11_SUBRESOURCE_DATA{
            .pSysMem = std::data(range),
            .SysMemPitch = 0u,
            .SysMemSlicePitch = 0u
        }
    ) {}
private:
    void bind(GFXPipeline& pipeline) override {
        GFX_THROW_FAILED_VOID(
            pipeline.context()->IASetIndexBuffer(
                data().Get(), indexFormat(), 0u
            )
        );
    }

    static consteval DXGI_FORMAT indexFormat() noexcept {
        if constexpr (std::is_same_v<MyIndex, unsigned char>) {
            return DXGI_FORMAT_R8_UINT;
        }
        else if constexpr (std::is_same_v<MyIndex, char>) {
            return DXGI_FORMAT_R8_SINT;
        }
        else if constexpr (std::is_same_v<MyIndex, unsigned short>) {
            return DXGI_FORMAT_R16_UINT;
        }
        else if constexpr (std::is_same_v<MyIndex, short>) {
            return DXGI_FORMAT_R16_SINT;
        }
        else if constexpr (std::is_same_v<MyIndex, unsigned int>) {
            return DXGI_FORMAT_R32_UINT;
        }
        else if constexpr (std::is_same_v<MyIndex, int>) {
            return DXGI_FORMAT_R32_SINT;
        }
        else {
            static_assert("unsupported index type detected."
                "supported index types are [unsigned char, char, unsigned short, short, unsigned int, int].");
        }
    }
};

template <class ValT>
class CBuffer : public Buffer {
public:
    using MyValue = ValT;

    template <std::ranges::contiguous_range R>
    CBuffer( GFXFactory factory,
        D3D11_USAGE usage, UINT CPUAccessFlags, R&& range
    ) : Buffer( std::move(factory),
        D3D11_BUFFER_DESC{
            .ByteWidth = static_cast<UINT>(
                std::size(range) * sizeof(MyValue)
            ),
            .Usage = usage,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = CPUAccessFlags,
            .MiscFlags = 0u,
            .StructureByteStride = sizeof(MyValue)
        },
        D3D11_SUBRESOURCE_DATA{
            .pSysMem = std::data(range),
            .SysMemPitch = 0u,
            .SysMemSlicePitch = 0u
        }
    ) {}

private:
    virtual void bind(GFXPipeline& pipeline) = 0;
};

template <class ValT>
class VSCBuffer : public CBuffer<ValT> {
public:
    using MyValue = ValT;
    using Buffer::data;

    template <std::ranges::contiguous_range R>
    VSCBuffer( GFXFactory factory,
        UINT slot, D3D11_USAGE usage, UINT CPUAccessFlags, R&& range
    ) : CBuffer<ValT>( std::move(factory), usage,
        CPUAccessFlags, std::forward<R>(range)
    ), slot_(slot) {}

private:
    void bind(GFXPipeline& pipeline) override {
        GFX_THROW_FAILED_VOID(
            pipeline.context()->VSSetConstantBuffers(
                slot_, 1u, data().GetAddressOf()
            )
        );
    }

    UINT slot_;
};

template <class ValT>
class PSCBuffer : public CBuffer<ValT> {
public:
    using MyValue = ValT;
    using Buffer::data;

    template <std::ranges::contiguous_range R>
    PSCBuffer( GFXFactory factory,
        UINT slot, D3D11_USAGE usage, UINT CPUAccessFlags, R&& range
    ) : CBuffer<ValT>( std::move(factory), usage,
        CPUAccessFlags, std::forward<R>(range)
    ), slot_(slot) {}

private:
    void bind(GFXPipeline& pipeline) override {
        GFX_THROW_FAILED_VOID(
            pipeline.context()->PSSetConstantBuffers(
                slot_, 1u, data().GetAddressOf()
            )
        );
    }

    UINT slot_;
};

#endif  // __Buffer