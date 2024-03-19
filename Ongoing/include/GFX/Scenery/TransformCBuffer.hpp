#ifndef __TransformCBuffer
#define __TransformCBuffer

#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/Core/Namespaces.hpp"

namespace gfx {
namespace scenery {

template <class TCBuffer>
class TransformCBuf : public TCBuffer {
public:
    TransformCBuf() = default;
    TransformCBuf(GFXFactory factory)
        : TCBuffer(factory, D3D11_USAGE_DYNAMIC,
            D3D11_CPU_ACCESS_WRITE, initialTransforms()
        ) {}

private:
    static std::vector<dx::XMMATRIX> initialTransforms() {
        return {dx::XMMatrixIdentity()};
    }
};

class VSTransformCBuf : public TransformCBuf< po::VSCBuffer<dx::XMMATRIX> > {};
class PSTransformCBuf : public TransformCBuf< po::PSCBuffer<dx::XMMATRIX> > {};

}   // namespace gfx::scenery
}   // namespace gfx

#endif  // __TransformCBuffer