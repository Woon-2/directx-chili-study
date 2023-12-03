#ifndef __Topology
#define __Topology

#include "Bindable.hpp"

#include "GFX/Core/Pipeline.hpp"
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"
#include <d3d11.h>

class Topology : public IBindable {
public:
    using MyValue = D3D11_PRIMITIVE_TOPOLOGY;

    Topology(const MyValue& topology)
        : data_(topology) {}

    void set(const MyValue& topology) {
        data_ = topology;
    }

    void set(MyValue&& topology) {
        data_ = std::move(topology);
    }

    const MyValue& get() const noexcept {
        return data_;
    }

private:
    void bind(GFXPipeline& pipeline) override {
        GFX_THROW_FAILED_VOID(
            pipeline.context()->IASetPrimitiveTopology(get())
        );
    }

    MyValue data_;
};

#endif  // __Topology