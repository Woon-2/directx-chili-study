#ifndef __TransformCBuffer
#define __TransformCBuffer

#include "GFX/PipelineObjects/Buffer.hpp"
#include "GFX/Core/GraphicsNamespaces.hpp"

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

class VSTransformCBuf : public TransformCBuf< VSCBuffer<dx::XMMATRIX> > {};
class PSTransformCBuf : public TransformCBuf< PSCBuffer<dx::XMMATRIX> > {};

#endif  // __TransformCBuffer