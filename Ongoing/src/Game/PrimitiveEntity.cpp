#include "Game/PrimitiveEntity.hpp"

std::vector<dx::XMMATRIX> PETransformCBuf::initialTransforms() {
    return std::vector<dx::XMMATRIX>{
        dx::XMMatrixTranspose(
            dx::XMMatrixIdentity()
            * dx::XMMatrixRotationY(0.7f)
            * dx::XMMatrixRotationX(-0.4f)
            * dx::XMMatrixTranslation( 0.f, 0.f, 4.f )
            * dx::XMMatrixPerspectiveLH( 1.f, 3.f/4.f, 0.5f, 10.f )
        )
    };
}

std::vector<PEFaceColorData> PEColorCBuf::initialColors() {
    return std::vector<PEFaceColorData>{{
        GFXColor{1.f, 0.f, 1.f, 1.f},
        GFXColor{1.f, 0.f, 0.f, 1.f},
        GFXColor{0.f, 1.f, 0.f, 1.f},
        GFXColor{0.f, 0.f, 1.f, 1.f},
        GFXColor{1.f, 1.f, 0.f, 1.f},
        GFXColor{0.f, 1.f, 1.f, 1.f}
    }};
}

std::vector<D3D11_INPUT_ELEMENT_DESC>
PEVertexShader::inputElemDescs() {
    return std::vector<D3D11_INPUT_ELEMENT_DESC>{
        { .SemanticName = "Position",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
}

void PEDrawContext::drawCall(GFXPipeline& pipeline) const {
    // first adopt transform
    assert( mappedStorage_->get(IDTransCBuf_).has_value() );

    auto transCBuf = static_cast<PETransformCBuf*>(
        mappedStorage_->get(IDTransCBuf_).value()
    );

    transCBuf->dynamicUpdate(pipeline, [this](){ return trans_.data(); });
    // then draw
    DrawContextIndexed::indexedDrawCall(pipeline);
}