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

    GFXCMDSummarizer(const GFXCMDLogger& logger);
    GFXCMDSummarizer(const GFXCMDLogger& logger,
        GFXCMDSourceCategory ctRenderer,
        GFXCMDSourceCategory ctDrawComponent
    );

    template <class ... PlaceHolders>
    MyString report(MyStringView format, PlaceHolders... phs) const {
        return std::vformat( format, std::make_format_args( formatGet(phs)... ) );
    }

    bool map(IDFrame id, std::size_t val);
    bool map(IDDrawComponent id, const IDrawComponent* val);
    bool map(IDRenderer id, const Renderer* val);

    const GFXCMDSourceCategory& categoryRenderer() const noexcept {
        return categoryRenderer_;
    }

    const GFXCMDSourceCategory& categoryDrawComponent() const noexcept {
        return categoryDrawComponent_;
    }

    void update(PHTotalCreateCnt);
    void update(PHTotalBindCnt);
    void update(PHTotalDrawCnt);
    void update(PHTotalCreateFCnt);
    void update(PHTotalBindFCnt);
    void update(PHTotalDrawFCnt);
    void update(PHRendererCreateCnt);
    void update(PHRendererBindCnt);
    void update(PHRendererDrawCnt);
    void update(PHRendererCreateFCnt);
    void update(PHRendererBindFCnt);
    void update(PHRendererDrawFCnt);
    void update(PHDrawComponentCreateCnt);
    void update(PHDrawComponentBindCnt);
    void update(PHDrawComponentDrawCnt);
    void update(PHDrawComponentCreateFCnt);
    void update(PHDrawComponentBindFCnt);
    void update(PHDrawComponentDrawFCnt);
    void update(PHIDRenderer, IDRenderer val);
    void update(PHIDDrawComponent, IDDrawComponent val);
    void update(PHIDFrame, IDFrame val);
    void update(PHNFrameSample, std::size_t val);

    GFXCMDLogger::Count get(PHTotalCreateCnt) const;
    GFXCMDLogger::Count get(PHTotalBindCnt) const;
    GFXCMDLogger::Count get(PHTotalDrawCnt) const;
    GFXCMDLogger::FloatCount get(PHTotalCreateFCnt) const;
    GFXCMDLogger::FloatCount get(PHTotalBindFCnt) const;
    GFXCMDLogger::FloatCount get(PHTotalDrawFCnt) const;
    GFXCMDLogger::Count get(PHRendererCreateCnt) const;
    GFXCMDLogger::Count get(PHRendererBindCnt) const;
    GFXCMDLogger::Count get(PHRendererDrawCnt) const;
    GFXCMDLogger::FloatCount get(PHRendererCreateFCnt) const;
    GFXCMDLogger::FloatCount get(PHRendererBindFCnt) const;
    GFXCMDLogger::FloatCount get(PHRendererDrawFCnt) const;
    GFXCMDLogger::Count get(PHDrawComponentCreateCnt) const;
    GFXCMDLogger::Count get(PHDrawComponentBindCnt) const;
    GFXCMDLogger::Count get(PHDrawComponentDrawCnt) const;
    GFXCMDLogger::FloatCount get(PHDrawComponentCreateFCnt) const;
    GFXCMDLogger::FloatCount get(PHDrawComponentBindFCnt) const;
    GFXCMDLogger::FloatCount get(PHDrawComponentDrawFCnt) const;
    const IDFrame get(PHIDFrame) const;
    const IDRenderer get(PHIDRenderer) const;
    const IDDrawComponent get(PHIDDrawComponent) const;
    std::size_t get(PHNFrameSample) const;

private:
    GFXCMDLogger::Count getCMDCnt(GFXCMDType cmdType) const;
    GFXCMDLogger::FloatCount getCMDCntF(GFXCMDType cmdType) const;
    GFXCMDLogger::Count getRCMDCnt(GFXCMDType cmdType) const;
    GFXCMDLogger::FloatCount getRCMDCntF(GFXCMDType cmdType) const;
    GFXCMDLogger::Count getDCCMDCnt(GFXCMDType cmdType) const;
    GFXCMDLogger::FloatCount getDCCMDCntF(GFXCMDType cmdType) const;

    template <class T, class Fn, class ... Args>
    void updateWithPropagation( std::optional<T>& val, Fn updater,
        Args& ... argsToInvalidate
    );

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
    void checkValueUpdated(T& val, MyStringView errMsg) const;

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