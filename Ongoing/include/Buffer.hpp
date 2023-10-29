#ifndef __Buffer
#define __Buffer

#include "ChiliWindow.hpp"

#include "Bindable.hpp"

#include "GraphicsNamespaces.hpp"
#include "GraphicsException.hpp"

#include <ranges>

class Buffer : public IBindable {
public:
    Buffer( const wrl::ComPtr<ID3D11Device> device,
        const D3D11_BUFFER_DESC& bufferDesc,
        const D3D11_SUBRESOURCE_DATA& subresourceData) {
        device->CreateBuffer(&bufferDesc, &subresourceData,
            &data_
        );
    }

    virtual void bind(GFXPipeline& pipeline) = 0;

    const wrl::ComPtr<ID3D11Buffer> data() {
        return data_;
    }

private:
    wrl::ComPtr<ID3D11Buffer> data_;
};


template <class VertexT>
class VertexBuffer : public Buffer {
public:
    using MyVertex = VertexT;

    template <std::ranges::contiguous_range R>
    VertexBuffer( const wrl::ComPtr<ID3D11Device> device,
        R&& range
    ) : Buffer( std::move(device),
            D3D11_BUFFER_DESC{
                .ByteWidth = static_cast<UINT>(
                    std::size(range) * sizeof(VertexT)
                ),
                .Usage = D3D11_USAGE_DEFAULT,
                .BindFlags = D3D11_BIND_VERTEX_BUFFER,
                .CPUAccessFlags = 0,
                .MiscFlags = 0,
                .StructureByteStride = sizeof(VertexT)
            },
            D3D11_SUBRESOURCE_DATA{
                .pSysMem = std::data(range),
                .SysMemPitch = 0,
                .SysMemSlicePitch = 0
            }
        ) {}

private:
    void bind(GFXPipeline& pipeline) override {
        const auto stride = static_cast<UINT>( sizeof(VertexT) );
        const auto offset = static_cast<UINT>( 0u );

        GFX_THROW_FAILED_VOID(
            pipeline.context()->IASetVertexBuffers(
                0u, 1u, data().GetAddressOf(),
                &stride, &offset
            )
        );
    } 
};

#endif  // __Buffer