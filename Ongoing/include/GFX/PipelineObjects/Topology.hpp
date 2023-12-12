#ifndef __Topology
#define __Topology

#include "Bindable.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <d3d11.h>
#include "GFX/Core/GraphicsNamespaces.hpp"
#include "GFX/Core/GraphicsException.hpp"

class TopologyBinder : public BinderInterface<TopologyBinder> {
public:
    friend class BinderInterface<TopologyBinder>;

private:
    void doBind(GFXPipeline& pipeline, D3D11_PRIMITIVE_TOPOLOGY topology) {
        GFX_THROW_FAILED_VOID(
            pipeline.context()->IASetPrimitiveTopology(topology)
        );
    }
};

class Topology : public IBindable,
    public LocalRebindInterface<Topology> {
public:
    using MyValue = D3D11_PRIMITIVE_TOPOLOGY;
    friend class LocalRebindInterface<Topology>;

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
    void bind(GFXPipeline& pipeline) override final {
        binder_.bind( pipeline, get() );
    }

    MyValue data_;
    TopologyBinder binder_;
};

#endif  // __Topology