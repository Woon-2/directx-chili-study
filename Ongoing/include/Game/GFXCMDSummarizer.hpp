#ifndef __GFXCMDSummarizer
#define __GFXCMDSummarizer

#include "GFXCMDLogger.hpp"

#include "Renderer.hpp"
#include "DrawComponent.hpp"

#include <variant>
#include <string>
#include <string_view>
#include <optional>
#include <unordered_map>

#define GFXCMDSUM getGFXCMDSummarizer()

class GFXCMDSummarizer {
public:
    using MyChar = char;
    using MyString = std::basic_string<MyChar>;
    using MyStringView = std::basic_string_view<MyChar>;

    struct IDFrame {
    public:
        struct Hash {
            std::size_t operator()(IDFrame key) const noexcept {
                return std::hash<std::size_t>{}(key.data_);
            }
        };

        constexpr IDFrame()
            : data_(0u) {}

        explicit constexpr IDFrame(std::size_t id)
            : data_(id) {}

        std::size_t& data() noexcept {
            return data_;
        }

        std::size_t data() const noexcept {
            return data_;
        }

        friend auto operator<=>(IDFrame lhs, IDFrame rhs) = default;

    private:
        std::size_t data_;
    };

    struct IDRenderer {
    public:
        struct Hash {
            std::size_t operator()(const IDRenderer& key) const noexcept {
                return std::hash<MyString>{}(key.data_);
            }
        };

        constexpr IDRenderer()
            : data_() {}

        explicit constexpr IDRenderer(MyStringView id)
            : data_(id) {}

        MyString& data() noexcept {
            return data_;
        }

        const MyString& data() const noexcept {
            return data_;
        }

        friend auto operator<=>(const IDRenderer& lhs, const IDRenderer& rhs) = default;

    private:
        MyString data_;
    };

    struct IDDrawComponent {
    public:
        struct Hash {
            std::size_t operator()(IDDrawComponent key) const noexcept {
                return std::hash<MyString>{}(key.data_);
            }
        };

        constexpr IDDrawComponent()
            : data_() {}

        explicit constexpr IDDrawComponent(MyStringView id)
            : data_(id) {}

        MyString& data() noexcept {
            return data_;
        }

        const MyString& data() const noexcept {
            return data_;
        }

        friend auto operator<=>(const IDDrawComponent& lhs, const IDDrawComponent& rhs) = default;

    private:
        MyString data_;
    };

private:
    struct PHTotalCreateCnt : std::monostate {};
    struct PHTotalBindCnt : std::monostate {};
    struct PHTotalDrawCnt : std::monostate {};
    struct PHTotalCreateFCnt : std::monostate {};
    struct PHTotalBindFCnt : std::monostate {};
    struct PHTotalDrawFCnt : std::monostate {};
    struct PHRendererCreateCnt : std::monostate {};
    struct PHRendererBindCnt : std::monostate {};
    struct PHRendererDrawCnt : std::monostate {};
    struct PHRendererCreateFCnt : std::monostate {};
    struct PHRendererBindFCnt : std::monostate {};
    struct PHRendererDrawFCnt : std::monostate {};
    struct PHDrawComponentCreateCnt : std::monostate {};
    struct PHDrawComponentBindCnt : std::monostate {};
    struct PHDrawComponentDrawCnt : std::monostate {};
    struct PHDrawComponentCreateFCnt : std::monostate {};
    struct PHDrawComponentBindFCnt : std::monostate {};
    struct PHDrawComponentDrawFCnt : std::monostate {};
    struct PHIDFrame : std::monostate {};
    struct PHIDRenderer : std::monostate {};
    struct PHIDDrawComponent : std::monostate {};
    struct PHNFrameSample : std::monostate {};

    using PHVariant = std::variant<
        PHTotalCreateCnt, PHTotalBindCnt, PHTotalDrawCnt,
        PHTotalCreateFCnt, PHTotalBindFCnt, PHTotalDrawFCnt,
        PHRendererCreateCnt, PHRendererBindCnt, PHRendererDrawCnt,
        PHRendererCreateFCnt, PHRendererBindFCnt, PHRendererDrawFCnt,
        PHDrawComponentCreateCnt, PHDrawComponentBindCnt, PHDrawComponentDrawCnt,
        PHDrawComponentCreateFCnt, PHDrawComponentBindFCnt, PHDrawComponentDrawFCnt,
        PHIDFrame, PHIDRenderer, PHIDDrawComponent, PHNFrameSample
    >;

public:
    static const PHTotalCreateCnt phTotalCreateCnt;
    static const PHTotalBindCnt phTotalBindCnt;
    static const PHTotalDrawCnt phTotalDrawCnt;
    static const PHTotalCreateFCnt phTotalCreateFCnt;
    static const PHTotalBindFCnt phTotalBindFCnt;
    static const PHTotalDrawFCnt phTotalDrawFCnt;
    static const PHRendererCreateCnt phRendererCreateCnt;
    static const PHRendererBindCnt phRendererBindCnt;
    static const PHRendererDrawCnt phRendererDrawCnt;
    static const PHRendererCreateFCnt phRendererCreateFCnt;
    static const PHRendererBindFCnt phRendererBindFCnt;
    static const PHRendererDrawFCnt phRendererDrawFCnt;
    static const PHDrawComponentCreateCnt phDrawComponentCreateCnt;
    static const PHDrawComponentBindCnt phDrawComponentBindCnt;
    static const PHDrawComponentDrawCnt phDrawComponentDrawCnt;
    static const PHDrawComponentCreateFCnt phDrawComponentCreateFCnt;
    static const PHDrawComponentBindFCnt phDrawComponentBindFCnt;
    static const PHDrawComponentDrawFCnt phDrawComponentDrawFCnt;
    static const PHIDFrame phIDFrame;
    static const PHIDRenderer phIDRenderer;
    static const PHIDDrawComponent phIDDrawComponent;
    static const PHNFrameSample phNFrameSample;

    // all unspecified members are default constructed.
    GFXCMDSummarizer(const GFXCMDLogger& logger)
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

    GFXCMDSummarizer(const GFXCMDLogger& logger,
        GFXCMDSourceCategory ctRenderer,
        GFXCMDSourceCategory ctDrawComponent
    ) : pLogger_(&logger),
        categoryRenderer_(ctRenderer),
        categoryDrawComponent_(ctDrawComponent) {}

    template <class ... PlaceHolders>
    MyString report(MyStringView format, PlaceHolders... phs) const {
        return std::vformat( format, std::make_format_args( formatGet(phs)... ) );
    }

    bool map(IDFrame id, std::size_t val) {
        return frameMap_.try_emplace(id, val).second;
    }

    bool map(IDDrawComponent id, const IDrawComponent* val) {
        return drawComponentMap_.try_emplace(id, val).second;
    }

    bool map(IDRenderer id, const Renderer* val) {
        return rendererMap_.try_emplace(id, val).second;
    }

    const GFXCMDSourceCategory& categoryRenderer() const noexcept {
        return categoryRenderer_;
    }

    const GFXCMDSourceCategory& categoryDrawComponent() const noexcept {
        return categoryDrawComponent_;
    }

    void update(PHTotalCreateCnt) {
        updateWithPropagation( freshTotalCreateCount_, [this](auto& cnt) {
                cnt = getCMDCnt( GFXCMDType::Create );
            }, freshRendererCreateCount_, freshRendererCreateFloatCount_,
            freshDrawComponentCreateCount_, freshDrawComponentCreateFloatCount_,
            freshTotalCreateFloatCount_
        );
    }

    void update(PHTotalBindCnt) {
        updateWithPropagation( freshTotalBindCount_, [this](auto& cnt) {
                cnt = getCMDCnt( GFXCMDType::Bind );
            }, freshRendererBindCount_, freshRendererBindFloatCount_,
            freshDrawComponentBindCount_, freshDrawComponentBindFloatCount_,
            freshTotalBindFloatCount_
        );
    }

    void update(PHTotalDrawCnt) {
        updateWithPropagation( freshTotalDrawCount_, [this](auto& cnt) {
                cnt = getCMDCnt( GFXCMDType::Draw );
            }, freshRendererDrawCount_, freshRendererDrawFloatCount_,
            freshDrawComponentDrawCount_, freshDrawComponentDrawFloatCount_,
            freshTotalDrawFloatCount_
        );
    }

    void update(PHTotalCreateFCnt) {
        updateWithPropagation( freshTotalCreateFloatCount_, [this](auto& cnt) {
                cnt = getCMDCntF( GFXCMDType::Create );
            }, freshRendererCreateCount_, freshRendererCreateFloatCount_,
            freshDrawComponentCreateCount_, freshDrawComponentCreateFloatCount_,
            freshTotalCreateCount_
        );
    }

    void update(PHTotalBindFCnt) {
        updateWithPropagation( freshTotalBindFloatCount_, [this](auto& cnt) {
                cnt = getCMDCntF( GFXCMDType::Bind );
            }, freshRendererBindCount_, freshRendererBindFloatCount_,
            freshDrawComponentBindCount_, freshDrawComponentBindFloatCount_,
            freshTotalBindCount_
        );
    }

    void update(PHTotalDrawFCnt) {
        updateWithPropagation( freshTotalDrawFloatCount_, [this](auto& cnt) {
                cnt = getCMDCntF( GFXCMDType::Draw );
            }, freshRendererDrawCount_, freshRendererDrawFloatCount_,
            freshDrawComponentDrawCount_, freshDrawComponentDrawFloatCount_,
            freshTotalDrawCount_
        );
    }

    void update(PHRendererCreateCnt) {
        updateWithPropagation( freshRendererCreateCount_, [this](auto& cnt) {
                cnt = getRCMDCnt( GFXCMDType::Create );
            }, freshTotalCreateCount_, freshTotalCreateFloatCount_,
            freshRendererCreateFloatCount_
        );
    }

    void update(PHRendererBindCnt) {
        updateWithPropagation( freshRendererBindCount_, [this](auto& cnt) {
                cnt = getRCMDCnt( GFXCMDType::Bind );
            }, freshTotalBindCount_, freshTotalBindFloatCount_,
            freshRendererBindFloatCount_
        );
    }

    void update(PHRendererDrawCnt) {
        updateWithPropagation( freshRendererDrawCount_, [this](auto& cnt) {
                cnt = getRCMDCnt( GFXCMDType::Draw );
            }, freshTotalDrawCount_, freshTotalDrawFloatCount_,
            freshRendererDrawFloatCount_
        );
    }

    void update(PHRendererCreateFCnt) {
        updateWithPropagation( freshRendererCreateFloatCount_, [this](auto& cnt) {
                cnt = getRCMDCntF( GFXCMDType::Create );
            }, freshTotalCreateCount_, freshTotalCreateFloatCount_,
            freshRendererCreateCount_
        );
    }

    void update(PHRendererBindFCnt) {
        updateWithPropagation( freshRendererBindFloatCount_, [this](auto& cnt) {
                cnt = getRCMDCntF( GFXCMDType::Bind );
            }, freshTotalBindCount_, freshTotalBindFloatCount_,
            freshRendererBindCount_
        );
    }

    void update(PHRendererDrawFCnt) {
        updateWithPropagation( freshRendererDrawFloatCount_, [this](auto& cnt) {
                cnt = getRCMDCntF( GFXCMDType::Draw );
            }, freshTotalDrawCount_, freshTotalDrawFloatCount_,
            freshRendererDrawCount_
        );
    }

    void update(PHDrawComponentCreateCnt) {
        updateWithPropagation( freshDrawComponentCreateCount_, [this](auto& cnt) {
                cnt = getDCCMDCnt( GFXCMDType::Create );
            }, freshTotalCreateCount_, freshTotalCreateFloatCount_
        );
    }

    void update(PHDrawComponentBindCnt) {
        updateWithPropagation( freshDrawComponentBindCount_, [this](auto& cnt) {
                cnt = getDCCMDCnt( GFXCMDType::Bind );
            }, freshTotalBindCount_, freshTotalBindFloatCount_
        );
    }

    void update(PHDrawComponentDrawCnt) {
        updateWithPropagation( freshDrawComponentDrawCount_, [this](auto& cnt) {
                cnt = getDCCMDCnt( GFXCMDType::Draw );
            }, freshTotalDrawCount_, freshTotalDrawFloatCount_
        );
    }

    void update(PHDrawComponentCreateFCnt) {
        updateWithPropagation( freshDrawComponentCreateFloatCount_, [this](auto& cnt) {
                cnt = getDCCMDCntF( GFXCMDType::Create );
            }, freshTotalCreateCount_, freshTotalCreateFloatCount_
        );
    }

    void update(PHDrawComponentBindFCnt) {
        updateWithPropagation( freshDrawComponentBindFloatCount_, [this](auto& cnt) {
                cnt = getDCCMDCntF( GFXCMDType::Bind );
            }, freshTotalBindCount_, freshTotalBindFloatCount_
        );
    }

    void update(PHDrawComponentDrawFCnt) {
        updateWithPropagation( freshDrawComponentDrawFloatCount_, [this](auto& cnt) {
                cnt = getDCCMDCntF( GFXCMDType::Draw );
            }, freshTotalDrawCount_, freshTotalDrawFloatCount_
        );
    }

    void update(PHIDRenderer, IDRenderer val) {
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

    void update(PHIDDrawComponent, IDDrawComponent val) {
        updateWithPropagation( IDDrawComponent_, [val](auto& id) {
                id = std::move(val);
            }, freshDrawComponentCreateCount_, freshDrawComponentBindCount_,
            freshDrawComponentDrawCount_, freshDrawComponentCreateFloatCount_,
            freshDrawComponentBindFloatCount_, freshDrawComponentDrawFloatCount_
        );
    }

    void update(PHIDFrame, IDFrame val) {
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

    void update(PHNFrameSample, std::size_t val) {
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

    GFXCMDLogger::Count get(PHTotalCreateCnt) const {
        checkValueUpdated(freshTotalCreateCount_, "");

        return freshTotalCreateCount_.value();
    }

    GFXCMDLogger::Count get(PHTotalBindCnt) const {
        checkValueUpdated(freshTotalBindCount_, "");

        return freshTotalBindCount_.value();
    }

    GFXCMDLogger::Count get(PHTotalDrawCnt) const {
        checkValueUpdated(freshTotalDrawCount_, "");

        return freshTotalDrawCount_.value();
    }

    GFXCMDLogger::FloatCount get(PHTotalCreateFCnt) const {
        checkValueUpdated(freshTotalCreateFloatCount_, "");

        return freshTotalCreateFloatCount_.value();
    }

    GFXCMDLogger::FloatCount get(PHTotalBindFCnt) const {
        checkValueUpdated(freshTotalBindFloatCount_, "");

        return freshTotalBindFloatCount_.value();
    }

    GFXCMDLogger::FloatCount get(PHTotalDrawFCnt) const {
        checkValueUpdated(freshTotalDrawFloatCount_, "");

        return freshTotalDrawFloatCount_.value();
    }

    GFXCMDLogger::Count get(PHRendererCreateCnt) const {
        checkValueUpdated(freshRendererCreateCount_, "");

        return freshRendererCreateCount_.value();
    }

    GFXCMDLogger::Count get(PHRendererBindCnt) const {
        checkValueUpdated(freshRendererBindCount_, "");

        return freshRendererBindCount_.value();
    }

    GFXCMDLogger::Count get(PHRendererDrawCnt) const {
        checkValueUpdated(freshRendererDrawCount_, "");

        return freshRendererDrawCount_.value();
    }

    GFXCMDLogger::FloatCount get(PHRendererCreateFCnt) const {
        checkValueUpdated(freshRendererCreateFloatCount_, "");

        return freshRendererCreateFloatCount_.value();
    }

    GFXCMDLogger::FloatCount get(PHRendererBindFCnt) const {
        checkValueUpdated(freshRendererBindFloatCount_, "");

        return freshRendererBindFloatCount_.value();
    }

    GFXCMDLogger::FloatCount get(PHRendererDrawFCnt) const {
        checkValueUpdated(freshRendererDrawFloatCount_, "");

        return freshRendererDrawFloatCount_.value();
    }

    GFXCMDLogger::Count get(PHDrawComponentCreateCnt) const {
        checkValueUpdated(freshDrawComponentCreateCount_, "");

        return freshDrawComponentCreateCount_.value();
    }

    GFXCMDLogger::Count get(PHDrawComponentBindCnt) const {
        checkValueUpdated(freshDrawComponentBindCount_, "");

        return freshDrawComponentBindCount_.value();
    }

    GFXCMDLogger::Count get(PHDrawComponentDrawCnt) const {
        checkValueUpdated(freshDrawComponentDrawCount_, "");

        return freshDrawComponentDrawCount_.value();
    }

    GFXCMDLogger::FloatCount get(PHDrawComponentCreateFCnt) const {
        checkValueUpdated(freshDrawComponentCreateFloatCount_, "");

        return freshDrawComponentCreateFloatCount_.value();
    }

    GFXCMDLogger::FloatCount get(PHDrawComponentBindFCnt) const {
        checkValueUpdated(freshDrawComponentBindFloatCount_, "");

        return freshDrawComponentBindFloatCount_.value();
    }

    GFXCMDLogger::FloatCount get(PHDrawComponentDrawFCnt) const {
        checkValueUpdated(freshDrawComponentDrawFloatCount_, "");

        return freshDrawComponentDrawFloatCount_.value();
    }

    const IDFrame get(PHIDFrame) const {
        checkValueUpdated(IDFrame_, "");

        return IDFrame_.value();
    }

    const IDRenderer get(PHIDRenderer) const {
        checkValueUpdated(IDRenderer_, "");

        return IDRenderer_.value();
    }

    const IDDrawComponent get(PHIDDrawComponent) const {
        checkValueUpdated(IDDrawComponent_, "");

        return IDDrawComponent_.value();
    }

    std::size_t get(PHNFrameSample) const {
        checkValueUpdated(freshNFrameSample_, "");

        return freshNFrameSample_.value();
    }

private:
    GFXCMDLogger::Count getCMDCnt(GFXCMDType cmdType) const {
        if (!freshNFrameSample_.has_value()) {
            return pLogger_->avCMDCnt(cmdType);
        }
        return pLogger_->avCMDCnt(cmdType, freshNFrameSample_.value());
    }

    GFXCMDLogger::FloatCount getCMDCntF(GFXCMDType cmdType) const {
        if (!freshNFrameSample_.has_value()) {
            return pLogger_->avCMDCntF(cmdType);
        }
        return pLogger_->avCMDCntF(cmdType, freshNFrameSample_.value());
    }

    GFXCMDLogger::Count getRCMDCnt(GFXCMDType cmdType) const {
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

    GFXCMDLogger::FloatCount getRCMDCntF(
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

    GFXCMDLogger::Count getDCCMDCnt(
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

    GFXCMDLogger::FloatCount getDCCMDCntF(
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
    void updateWithPropagation( std::optional<T>& val, Fn updater,
        Args& ... argsToInvalidate
    ) {
        auto last = val;

        std::invoke(updater, val);

        if ( last.has_value() && last.value() != val.value() ) {
            ( argsToInvalidate.reset(), ... );
        }
    }

    template <class PH>
    decltype(auto) formatGet(PH placeHolder) const {
        return get(placeHolder);
    }

    template <class PHID>
        requires std::same_as< std::remove_cvref_t<PHID>, PHIDFrame >
            || std::same_as< std::remove_cvref_t<PHID>, PHIDRenderer >
            || std::same_as< std::remove_cvref_t<PHID>, PHIDDrawComponent >
    auto formatGet(PHID placeHolder) const {
        return get(placeHolder).data();
    }

    template <class T>
    void checkValueUpdated(T& val, MyStringView errMsg) const {
        if (!val.has_value()) {
            // do error handling
        }
    }

    const GFXCMDLogger* pLogger_;

    GFXCMDSourceCategory categoryRenderer_;
    GFXCMDSourceCategory categoryDrawComponent_;

    std::optional<GFXCMDLogger::Count> freshTotalCreateCount_;
    std::optional<GFXCMDLogger::Count> freshTotalBindCount_;
    std::optional<GFXCMDLogger::Count> freshTotalDrawCount_;
    std::optional<GFXCMDLogger::FloatCount> freshTotalCreateFloatCount_;
    std::optional<GFXCMDLogger::FloatCount> freshTotalBindFloatCount_;
    std::optional<GFXCMDLogger::FloatCount> freshTotalDrawFloatCount_;

    std::optional<GFXCMDLogger::Count> freshRendererCreateCount_;
    std::optional<GFXCMDLogger::Count> freshRendererBindCount_;
    std::optional<GFXCMDLogger::Count> freshRendererDrawCount_;
    std::optional<GFXCMDLogger::FloatCount> freshRendererCreateFloatCount_;
    std::optional<GFXCMDLogger::FloatCount> freshRendererBindFloatCount_;
    std::optional<GFXCMDLogger::FloatCount> freshRendererDrawFloatCount_;

    std::optional<GFXCMDLogger::Count> freshDrawComponentCreateCount_;
    std::optional<GFXCMDLogger::Count> freshDrawComponentBindCount_;
    std::optional<GFXCMDLogger::Count> freshDrawComponentDrawCount_;
    std::optional<GFXCMDLogger::FloatCount> freshDrawComponentCreateFloatCount_;
    std::optional<GFXCMDLogger::FloatCount> freshDrawComponentBindFloatCount_;
    std::optional<GFXCMDLogger::FloatCount> freshDrawComponentDrawFloatCount_;

    std::optional<std::size_t> freshNFrameSample_;
    std::optional<const Renderer*> freshCurRenderer_;
    std::optional<const IDrawComponent*> freshCurDrawComponent_;
    
    std::optional<IDFrame> IDFrame_;
    std::optional<IDRenderer> IDRenderer_;
    std::optional<IDDrawComponent> IDDrawComponent_;

    std::unordered_map<IDFrame, std::size_t, IDFrame::Hash> frameMap_;
    std::unordered_map<IDRenderer, const Renderer*, IDRenderer::Hash> rendererMap_;
    std::unordered_map< IDDrawComponent,
        const IDrawComponent*, IDDrawComponent::Hash
    > drawComponentMap_;
};

GFXCMDSummarizer& getGFXCMDSummarizer();

#endif  // __GFXCMDSummarizer