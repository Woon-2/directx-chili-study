#include "Game/GFXCMDSummarizer.hpp"

#include <optional>

const GFXCMDSummarizer::PHTotalCreateCnt 
    GFXCMDSummarizer::phTotalCreateCnt;
const GFXCMDSummarizer::PHTotalBindCnt 
    GFXCMDSummarizer::phTotalBindCnt;
const GFXCMDSummarizer::PHTotalDrawCnt 
    GFXCMDSummarizer::phTotalDrawCnt;
const GFXCMDSummarizer::PHTotalCreateFCnt 
    GFXCMDSummarizer::phTotalCreateFCnt;
const GFXCMDSummarizer::PHTotalBindFCnt 
    GFXCMDSummarizer::phTotalBindFCnt;
const GFXCMDSummarizer::PHTotalDrawFCnt 
    GFXCMDSummarizer::phTotalDrawFCnt;
const GFXCMDSummarizer::PHRendererCreateCnt 
    GFXCMDSummarizer::phRendererCreateCnt;
const GFXCMDSummarizer::PHRendererBindCnt 
    GFXCMDSummarizer::phRendererBindCnt;
const GFXCMDSummarizer::PHRendererDrawCnt 
    GFXCMDSummarizer::phRendererDrawCnt;
const GFXCMDSummarizer::PHRendererCreateFCnt 
    GFXCMDSummarizer::phRendererCreateFCnt;
const GFXCMDSummarizer::PHRendererBindFCnt 
    GFXCMDSummarizer::phRendererBindFCnt;
const GFXCMDSummarizer::PHRendererDrawFCnt 
    GFXCMDSummarizer::phRendererDrawFCnt;
const GFXCMDSummarizer::PHDrawComponentCreateCnt 
    GFXCMDSummarizer::phDrawComponentCreateCnt;
const GFXCMDSummarizer::PHDrawComponentBindCnt 
    GFXCMDSummarizer::phDrawComponentBindCnt;
const GFXCMDSummarizer::PHDrawComponentDrawCnt 
    GFXCMDSummarizer::phDrawComponentDrawCnt;
const GFXCMDSummarizer::PHDrawComponentCreateFCnt 
    GFXCMDSummarizer::phDrawComponentCreateFCnt;
const GFXCMDSummarizer::PHDrawComponentBindFCnt 
    GFXCMDSummarizer::phDrawComponentBindFCnt;
const GFXCMDSummarizer::PHDrawComponentDrawFCnt 
    GFXCMDSummarizer::phDrawComponentDrawFCnt;
const GFXCMDSummarizer::PHIDFrame 
    GFXCMDSummarizer::phIDFrame;
const GFXCMDSummarizer::PHIDRenderer 
    GFXCMDSummarizer::phIDRenderer;
const GFXCMDSummarizer::PHIDDrawComponent 
    GFXCMDSummarizer::phIDDrawComponent;
const GFXCMDSummarizer::PHNFrameSample 
    GFXCMDSummarizer::phNFrameSample;

GFXCMDSummarizer& getGFXCMDSummarizer() {
    static auto inst = std::optional<GFXCMDSummarizer>();

    if (!inst.has_value()) {
        inst = GFXCMDSummarizer( getGFXCMDLogger() );
    }

    return inst.value();
}