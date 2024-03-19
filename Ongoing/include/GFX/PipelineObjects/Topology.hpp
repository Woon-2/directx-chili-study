#ifndef __Topology
#define __Topology

#include "PipelineObject.hpp"
#include "GFX/Core/Pipeline.hpp"

#include <d3d11.h>
#include "GFX/Core/Namespaces.hpp"
#include "GFX/Core/Exception.hpp"

namespace gfx {
namespace po {

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

class Topology : public IPipelineObject,
    public LocalRebindInterface<Topology> {
public:
    using MyValue = D3D11_PRIMITIVE_TOPOLOGY;
    friend class LocalRebindInterface<Topology>;

    Topology( const MyValue& topology
    #ifdef ACTIVATE_BINDABLE_LOG
        , bool enableLogOnCreation = true
    #endif
    ) :
    #ifdef ACTIVATE_BINDABLE_LOG
        logComponent_( this, GFXCMDSourceCategory("Topology") ),
    #endif
        data_(topology) {
    #ifdef ACTIVATE_BINDABLE_LOG
        if (enableLogOnCreation) {
            logComponent_.enableLog();
        }
        logComponent_.logCreate();
    #endif
    }

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
        [[maybe_unused]] auto bBindOccured = binder_.bind(
            pipeline, get()
        );

    #ifdef ACTIVATE_BINDABLE_LOG
        if (bBindOccured) {
            logComponent_.logBind();
        }
    #endif 
    }

#ifdef ACTIVATE_BINDABLE_LOG
    IPipelineObject::LogComponent logComponent_;
#endif
    MyValue data_;
    TopologyBinder binder_;
};

}   // namespace gfx
}   // namespace po

#endif  // __Topology