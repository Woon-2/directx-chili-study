#ifndef __PointLight
#define __PointLight

#include "GFX/Core/GraphicsNamespaces.hpp"

#include "GFX/PipelineObjects/Buffer.hpp"

struct BPPointLightDesc {
    dx::XMFLOAT3A pos;
    dx::XMFLOAT3 color;
    float attConst;
    float attLin;
    float attQuad;
};

class BPDynPointLight {
public:
    const BPPointLightDesc& lightDesc() const noexcept {
        return lightDesc_;
    }

    const dx::XMVECTOR VCALL pos() const noexcept {
        return dx::XMLoadFloat3(&lightDesc_.pos);
    }

    const dx::XMVECTOR VCALL color() const noexcept {
        return dx::XMLoadFloat3(&lightDesc_.color);
    }

    const float attConst() const noexcept {
        return lightDesc_.attConst;
    }

    const float attLin() const noexcept {
        return lightDesc_.attLin;
    }

    const float attQuad() const noexcept {
        return lightDesc_.attQuad;
    }

    void VCALL setPos(dx::XMVECTOR posVal) {
        dx::XMStoreFloat3A(&lightDesc_.pos, posVal);
    }

    void VCALL setColor(dx::XMVECTOR colorVal) {
        dx::XMStoreFloat3(&lightDesc_.color, colorVal);
    }

    void setAttConst(float attConstVal) {
        lightDesc_.attConst = attConstVal;
    }

    void setAttLin(float attLinVal) {
        lightDesc_.attLin = attLinVal;
    }

    void setAttQuad(float attQuadVal) {
        lightDesc_.attQuad = attQuadVal;
    }

private:
    PSCBuffer<BPPointLightDesc> mapped_;
    BPPointLightDesc lightDesc_;
};

#endif  // __PointLight