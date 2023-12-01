#ifndef __Buffer
#define __Buffer

#include "ChiliWindow.hpp"

#include "Bindable.hpp"
#include "Pipeline.hpp"
#include "GFXFactory.hpp"
#include <d3d11.h>

#include "GraphicsNamespaces.hpp"
#include "GraphicsException.hpp"

#include <ranges>
#include <type_traits>

class Buffer : public IBindable {
public:
    Buffer( GFXFactory factory,
        const D3D11_BUFFER_DESC& bufferDesc,
        const D3D11_SUBRESOURCE_DATA& subresourceData) {
        factory.device()->CreateBuffer(&bufferDesc, &subresourceData,
            &data_
        );
    }

    const wrl::ComPtr<ID3D11Buffer> data() {
        return data_;
    }

private:
    virtual void bind(GFXPipeline& pipeline) = 0;

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