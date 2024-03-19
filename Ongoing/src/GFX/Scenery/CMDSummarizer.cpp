#include "GFX/Scenery/CMDSummarizer.hpp"

#include <optional>

namespace gfx {
namespace scenery {

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

// all unspecified members are default constructed.
GFXCMDSummarizer::GFXCMDSummarizer(const GFXCMDLogger& logger)
    : GFXCMDSummarizer(logger,
#ifdef ACTIVATE_RENDERER_LOG
        Renderer::logCategory(),
#else
        "Renderer",
#endif
#ifdef ACTIVATE_DRAWCOMPONENT_LOG
        IDrawComponent::logCategory()
#else
        "DrawComponent"
#endif
    ) {}

GFXCMDSummarizer::GFXCMDSummarizer(const GFXCMDLogger& logger,
    GFXCMDSourceCategory ctRenderer,
    GFXCMDSourceCategory ctDrawComponent
) : pLogger_(&logger),
    categoryRenderer_(ctRenderer),
    categoryDrawComponent_(ctDrawComponent) {}

bool GFXCMDSummarizer::map(IDFrame id, std::size_t val) {
    return frameMap_.try_emplace(id, val).second;
}

bool GFXCMDSummarizer::map(
    IDDrawComponent id, const IDrawComponent* val
) {
    return drawComponentMap_.try_emplace(id, val).second;
}

bool GFXCMDSummarizer::map(IDRenderer id, const Renderer* val) {
    return rendererMap_.try_emplace(id, val).second;
}

void GFXCMDSummarizer::update(PHTotalCreateCnt) {
    updateWithPropagation( freshTotalCreateCount_, [this](auto& cnt) {
            cnt = getCMDCnt( GFXCMDType::Create );
        }, freshRendererCreateCount_, freshRendererCreateFloatCount_,
        freshDrawComponentCreateCount_, freshDrawComponentCreateFloatCount_,
        freshTotalCreateFloatCount_
    );
}

void GFXCMDSummarizer::update(PHTotalBindCnt) {
    updateWithPropagation( freshTotalBindCount_, [this](auto& cnt) {
            cnt = getCMDCnt( GFXCMDType::Bind );
        }, freshRendererBindCount_, freshRendererBindFloatCount_,
        freshDrawComponentBindCount_, freshDrawComponentBindFloatCount_,
        freshTotalBindFloatCount_
    );
}

void GFXCMDSummarizer::update(PHTotalDrawCnt) {
    updateWithPropagation( freshTotalDrawCount_, [this](auto& cnt) {
            cnt = getCMDCnt( GFXCMDType::Draw );
        }, freshRendererDrawCount_, freshRendererDrawFloatCount_,
        freshDrawComponentDrawCount_, freshDrawComponentDrawFloatCount_,
        freshTotalDrawFloatCount_
    );
}

void GFXCMDSummarizer::update(PHTotalCreateFCnt) {
    updateWithPropagation( freshTotalCreateFloatCount_, [this](auto& cnt) {
            cnt = getCMDCntF( GFXCMDType::Create );
        }, freshRendererCreateCount_, freshRendererCreateFloatCount_,
        freshDrawComponentCreateCount_, freshDrawComponentCreateFloatCount_,
        freshTotalCreateCount_
    );
}

void GFXCMDSummarizer::update(PHTotalBindFCnt) {
    updateWithPropagation( freshTotalBindFloatCount_, [this](auto& cnt) {
            cnt = getCMDCntF( GFXCMDType::Bind );
        }, freshRendererBindCount_, freshRendererBindFloatCount_,
        freshDrawComponentBindCount_, freshDrawComponentBindFloatCount_,
        freshTotalBindCount_
    );
}

void GFXCMDSummarizer::update(PHTotalDrawFCnt) {
    updateWithPropagation( freshTotalDrawFloatCount_, [this](auto& cnt) {
            cnt = getCMDCntF( GFXCMDType::Draw );
        }, freshRendererDrawCount_, freshRendererDrawFloatCount_,
        freshDrawComponentDrawCount_, freshDrawComponentDrawFloatCount_,
        freshTotalDrawCount_
    );
}

void GFXCMDSummarizer::update(PHRendererCreateCnt) {
    updateWithPropagation( freshRendererCreateCount_, [this](auto& cnt) {
            cnt = getRCMDCnt( GFXCMDType::Create );
        }, freshTotalCreateCount_, freshTotalCreateFloatCount_,
        freshRendererCreateFloatCount_
    );
}

void GFXCMDSummarizer::update(PHRendererBindCnt) {
    updateWithPropagation( freshRendererBindCount_, [this](auto& cnt) {
            cnt = getRCMDCnt( GFXCMDType::Bind );
        }, freshTotalBindCount_, freshTotalBindFloatCount_,
        freshRendererBindFloatCount_
    );
}

void GFXCMDSummarizer::update(PHRendererDrawCnt) {
    updateWithPropagation( freshRendererDrawCount_, [this](auto& cnt) {
            cnt = getRCMDCnt( GFXCMDType::Draw );
        }, freshTotalDrawCount_, freshTotalDrawFloatCount_,
        freshRendererDrawFloatCount_
    );
}

void GFXCMDSummarizer::update(PHRendererCreateFCnt) {
    updateWithPropagation( freshRendererCreateFloatCount_, [this](auto& cnt) {
            cnt = getRCMDCntF( GFXCMDType::Create );
        }, freshTotalCreateCount_, freshTotalCreateFloatCount_,
        freshRendererCreateCount_
    );
}

void GFXCMDSummarizer::update(PHRendererBindFCnt) {
    updateWithPropagation( freshRendererBindFloatCount_, [this](auto& cnt) {
            cnt = getRCMDCntF( GFXCMDType::Bind );
        }, freshTotalBindCount_, freshTotalBindFloatCount_,
        freshRendererBindCount_
    );
}

void GFXCMDSummarizer::update(PHRendererDrawFCnt) {
    updateWithPropagation( freshRendererDrawFloatCount_, [this](auto& cnt) {
            cnt = getRCMDCntF( GFXCMDType::Draw );
        }, freshTotalDrawCount_, freshTotalDrawFloatCount_,
        freshRendererDrawCount_
    );
}

void GFXCMDSummarizer::update(PHDrawComponentCreateCnt) {
    updateWithPropagation( freshDrawComponentCreateCount_, [this](auto& cnt) {
            cnt = getDCCMDCnt( GFXCMDType::Create );
        }, freshTotalCreateCount_, freshTotalCreateFloatCount_
    );
}

void GFXCMDSummarizer::update(PHDrawComponentBindCnt) {
    updateWithPropagation( freshDrawComponentBindCount_, [this](auto& cnt) {
            cnt = getDCCMDCnt( GFXCMDType::Bind );
        }, freshTotalBindCount_, freshTotalBindFloatCount_
    );
}

void GFXCMDSummarizer::update(PHDrawComponentDrawCnt) {
    updateWithPropagation( freshDrawComponentDrawCount_, [this](auto& cnt) {
            cnt = getDCCMDCnt( GFXCMDType::Draw );
        }, freshTotalDrawCount_, freshTotalDrawFloatCount_
    );
}

void GFXCMDSummarizer::update(PHDrawComponentCreateFCnt) {
    updateWithPropagation( freshDrawComponentCreateFloatCount_, [this](auto& cnt) {
            cnt = getDCCMDCntF( GFXCMDType::Create );
        }, freshTotalCreateCount_, freshTotalCreateFloatCount_
    );
}

void GFXCMDSummarizer::update(PHDrawComponentBindFCnt) {
    updateWithPropagation( freshDrawComponentBindFloatCount_, [this](auto& cnt) {
            cnt = getDCCMDCntF( GFXCMDType::Bind );
        }, freshTotalBindCount_, freshTotalBindFloatCount_
    );
}

void GFXCMDSummarizer::update(PHDrawComponentDrawFCnt) {
    updateWithPropagation( freshDrawComponentDrawFloatCount_, [this](auto& cnt) {
            cnt = getDCCMDCntF( GFXCMDType::Draw );
        }, freshTotalDrawCount_, freshTotalDrawFloatCount_
    );
}

void GFXCMDSummarizer::update(PHIDRenderer, IDRenderer val) {
    updateWithPropagation( IDRenderer_, [this, val](auto& id) {
            if ( !rendererMap_.contains(val) ) {
                // received undefined id,
                // do proper error handling.
            }

            id = std::move(val);
            freshCurRenderer_ = rendererMap_.at(id.value());
        }, freshRendererCreateCount_, freshRendererBindCount_,
        freshRendererDrawCount_, freshRendererCreateFloatCount_,
        freshRendererBindFloatCount_, freshRendererDrawFloatCount_
    );
}

void GFXCMDSummarizer::update(PHIDDrawComponent, IDDrawComponent val) {
    updateWithPropagation( IDDrawComponent_, [val](auto& id) {
            id = std::move(val);
        }, freshDrawComponentCreateCount_, freshDrawComponentBindCount_,
        freshDrawComponentDrawCount_, freshDrawComponentCreateFloatCount_,
        freshDrawComponentBindFloatCount_, freshDrawComponentDrawFloatCount_
    );
}

void GFXCMDSummarizer::update(PHIDFrame, IDFrame val) {
    updateWithPropagation( IDFrame_, [val](auto& id) {
            id = std::move(val);
        }, freshTotalCreateCount_, freshTotalBindCount_,
        freshTotalDrawCount_, freshTotalCreateFloatCount_,
        freshTotalBindFloatCount_, freshTotalDrawFloatCount_,
        freshRendererCreateCount_, freshRendererBindCount_,
        freshRendererDrawCount_, freshRendererCreateFloatCount_,
        freshRendererBindFloatCount_, freshRendererDrawFloatCount_,
        freshDrawComponentCreateCount_, freshDrawComponentBindCount_,
        freshDrawComponentDrawCount_, freshDrawComponentCreateFloatCount_,
        freshDrawComponentBindFloatCount_, freshDrawComponentDrawFloatCount_
    );
}

void GFXCMDSummarizer::update(PHNFrameSample, std::size_t val) {
    updateWithPropagation( freshNFrameSample_, [val](auto& nFrameSample) {
            nFrameSample = val;
        }, freshTotalCreateCount_, freshTotalBindCount_,
        freshTotalDrawCount_, freshTotalCreateFloatCount_,
        freshTotalBindFloatCount_, freshTotalDrawFloatCount_,
        freshRendererCreateCount_, freshRendererBindCount_,
        freshRendererDrawCount_, freshRendererCreateFloatCount_,
        freshRendererBindFloatCount_, freshRendererDrawFloatCount_,
        freshDrawComponentCreateCount_, freshDrawComponentBindCount_,
        freshDrawComponentDrawCount_, freshDrawComponentCreateFloatCount_,
        freshDrawComponentBindFloatCount_, freshDrawComponentDrawFloatCount_
    );
}

GFXCMDLogger::Count GFXCMDSummarizer::get(PHTotalCreateCnt) const {
    checkValueUpdated(freshTotalCreateCount_, "");

    return freshTotalCreateCount_.value();
}

GFXCMDLogger::Count GFXCMDSummarizer::get(PHTotalBindCnt) const {
    checkValueUpdated(freshTotalBindCount_, "");

    return freshTotalBindCount_.value();
}

GFXCMDLogger::Count GFXCMDSummarizer::get(PHTotalDrawCnt) const {
    checkValueUpdated(freshTotalDrawCount_, "");

    return freshTotalDrawCount_.value();
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::get(PHTotalCreateFCnt) const {
    checkValueUpdated(freshTotalCreateFloatCount_, "");

    return freshTotalCreateFloatCount_.value();
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::get(PHTotalBindFCnt) const {
    checkValueUpdated(freshTotalBindFloatCount_, "");

    return freshTotalBindFloatCount_.value();
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::get(PHTotalDrawFCnt) const {
    checkValueUpdated(freshTotalDrawFloatCount_, "");

    return freshTotalDrawFloatCount_.value();
}

GFXCMDLogger::Count GFXCMDSummarizer::get(PHRendererCreateCnt) const {
    checkValueUpdated(freshRendererCreateCount_, "");

    return freshRendererCreateCount_.value();
}

GFXCMDLogger::Count GFXCMDSummarizer::get(PHRendererBindCnt) const {
    checkValueUpdated(freshRendererBindCount_, "");

    return freshRendererBindCount_.value();
}

GFXCMDLogger::Count GFXCMDSummarizer::get(PHRendererDrawCnt) const {
    checkValueUpdated(freshRendererDrawCount_, "");

    return freshRendererDrawCount_.value();
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::get(PHRendererCreateFCnt) const {
    checkValueUpdated(freshRendererCreateFloatCount_, "");

    return freshRendererCreateFloatCount_.value();
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::get(PHRendererBindFCnt) const {
    checkValueUpdated(freshRendererBindFloatCount_, "");

    return freshRendererBindFloatCount_.value();
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::get(PHRendererDrawFCnt) const {
    checkValueUpdated(freshRendererDrawFloatCount_, "");

    return freshRendererDrawFloatCount_.value();
}

GFXCMDLogger::Count GFXCMDSummarizer::get(PHDrawComponentCreateCnt) const {
    checkValueUpdated(freshDrawComponentCreateCount_, "");

    return freshDrawComponentCreateCount_.value();
}

GFXCMDLogger::Count GFXCMDSummarizer::get(PHDrawComponentBindCnt) const {
    checkValueUpdated(freshDrawComponentBindCount_, "");

    return freshDrawComponentBindCount_.value();
}

GFXCMDLogger::Count GFXCMDSummarizer::get(PHDrawComponentDrawCnt) const {
    checkValueUpdated(freshDrawComponentDrawCount_, "");

    return freshDrawComponentDrawCount_.value();
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::get(PHDrawComponentCreateFCnt) const {
    checkValueUpdated(freshDrawComponentCreateFloatCount_, "");

    return freshDrawComponentCreateFloatCount_.value();
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::get(PHDrawComponentBindFCnt) const {
    checkValueUpdated(freshDrawComponentBindFloatCount_, "");

    return freshDrawComponentBindFloatCount_.value();
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::get(PHDrawComponentDrawFCnt) const {
    checkValueUpdated(freshDrawComponentDrawFloatCount_, "");

    return freshDrawComponentDrawFloatCount_.value();
}

const GFXCMDSummarizer::IDFrame GFXCMDSummarizer::get(PHIDFrame) const {
    checkValueUpdated(IDFrame_, "");

    return IDFrame_.value();
}

const GFXCMDSummarizer::IDRenderer GFXCMDSummarizer::get(PHIDRenderer) const {
    checkValueUpdated(IDRenderer_, "");

    return IDRenderer_.value();
}

const GFXCMDSummarizer::IDDrawComponent GFXCMDSummarizer::get(PHIDDrawComponent) const {
    checkValueUpdated(IDDrawComponent_, "");

    return IDDrawComponent_.value();
}

std::size_t GFXCMDSummarizer::get(PHNFrameSample) const {
    checkValueUpdated(freshNFrameSample_, "");

    return freshNFrameSample_.value();
}

GFXCMDLogger::Count GFXCMDSummarizer::getCMDCnt(GFXCMDType cmdType) const {
    if (!freshNFrameSample_.has_value()) {
        return pLogger_->avCMDCnt(cmdType);
    }
    return pLogger_->avCMDCnt(cmdType, freshNFrameSample_.value());
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::getCMDCntF(GFXCMDType cmdType) const {
    if (!freshNFrameSample_.has_value()) {
        return pLogger_->avCMDCntF(cmdType);
    }
    return pLogger_->avCMDCntF(cmdType, freshNFrameSample_.value());
}

GFXCMDLogger::Count GFXCMDSummarizer::getRCMDCnt(GFXCMDType cmdType) const {
    if (!freshCurRenderer_.has_value()) {
        // renderer is not set,
        // do proper error handling
        return 0u;
    }

    if (!freshNFrameSample_.has_value()) {
        return pLogger_->avCMDCnt( cmdType, GFXCMDSource{
            .category = categoryRenderer(),
            .pSource = freshCurRenderer_.value()
        } );
    }

    return pLogger_->avCMDCnt( cmdType, GFXCMDSource{
        .category = categoryRenderer(),
        .pSource = freshCurRenderer_.value()
    }, freshNFrameSample_.value() );
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::getRCMDCntF(
    GFXCMDType cmdType
) const {
    if (!freshCurRenderer_.has_value()) {
        // renderer is not set,
        // do proper error handling
        return 0u;
    }

    if (!freshNFrameSample_.has_value()) {
        return pLogger_->avCMDCntF( cmdType, GFXCMDSource{
            .category = categoryRenderer(),
            .pSource = freshCurRenderer_.value()
        } );
    }
    return pLogger_->avCMDCntF( cmdType, GFXCMDSource{
        .category = categoryRenderer(),
        .pSource = freshCurRenderer_.value()
    }, freshNFrameSample_.value() );
}

GFXCMDLogger::Count GFXCMDSummarizer::getDCCMDCnt(
    GFXCMDType cmdType
) const {
    if (!freshCurDrawComponent_.has_value()) {
        // DrawComponent is not set,
        // do proper error handling
        return 0u;
    }

    if (!freshNFrameSample_.has_value()) {
        return pLogger_->avCMDCnt( cmdType, GFXCMDSource{
            .category = categoryDrawComponent(),
            .pSource = freshCurDrawComponent_.value()
        } );
    }

    return pLogger_->avCMDCnt( cmdType, GFXCMDSource{
        .category = categoryDrawComponent(),
        .pSource = freshCurDrawComponent_.value()
    }, freshNFrameSample_.value() );
}

GFXCMDLogger::FloatCount GFXCMDSummarizer::getDCCMDCntF(
    GFXCMDType cmdType
) const {
    if (!freshCurDrawComponent_.has_value()) {
        // DrawComponent is not set,
        // do proper error handling
        return 0u;
    }

    if (!freshNFrameSample_.has_value()) {
        return pLogger_->avCMDCntF( cmdType, GFXCMDSource{
            .category = categoryDrawComponent(),
            .pSource = freshCurDrawComponent_.value()
        } );
    }

    return pLogger_->avCMDCntF( cmdType, GFXCMDSource{
        .category = categoryDrawComponent(),
        .pSource = freshCurDrawComponent_.value()
    }, freshNFrameSample_.value() );
}

template <class T, class Fn, class ... Args>
void GFXCMDSummarizer::updateWithPropagation(
    std::optional<T>& val, Fn updater,
    Args& ... argsToInvalidate
) {
    auto last = val;

    std::invoke(updater, val);

    if ( last.has_value() && last.value() != val.value() ) {
        ( argsToInvalidate.reset(), ... );
    }
}

template <class T>
void GFXCMDSummarizer::checkValueUpdated(T& val, MyStringView errMsg) const {
    if (!val.has_value()) {
        // do error handling
    }
}

GFXCMDSummarizer& getGFXCMDSummarizer() {
    static auto inst = std::optional<GFXCMDSummarizer>();

    if (!inst.has_value()) {
        inst = GFXCMDSummarizer( getGFXCMDLogger() );
    }

    return inst.value();
}

}  // namespace gfx::scenery
}  // namespace gfx