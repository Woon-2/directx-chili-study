#include "GFX/Scenery/CMDLogFileView.hpp"

#include <optional>

namespace gfx {
namespace scenery {

void GFXCMDLogFileView::report() {
    // should protect frameID from overflow later.
    static auto frameID = std::size_t(0u);

    ++frameID;

    GFXCMDSUM.update(GFXCMDSUM.phIDFrame, GFXCMDSummarizer::IDFrame(frameID));
    GFXCMDSUM.update(GFXCMDSUM.phNFrameSample, 1u);

    GFXCMDSUM.update(GFXCMDSUM.phTotalCreateCnt);
    GFXCMDSUM.update(GFXCMDSUM.phTotalBindCnt);
    GFXCMDSUM.update(GFXCMDSUM.phTotalDrawCnt);

    // There's a chance of having different character type
    // between GFXCMDSummarizer and GFXCMDLogFileView.
    // Thus the report string has to be converted.
    auto tmp = GFXCMDSUM.report( "Frame {} - Create: {} Bind: {} Draw: {}\n",
        GFXCMDSUM.phIDFrame, GFXCMDSUM.phTotalCreateCnt,
        GFXCMDSUM.phTotalBindCnt, GFXCMDSUM.phTotalDrawCnt
    );

    auto str = MyString();
    str.reserve(tmp.size());

    // convert the report string here.
    str.assign(tmp.begin(), tmp.end());

    // write report string on the output file.
    out_ << std::move(str);
}

GFXCMDLogFileView& getGFXCMDLogFileView() {
    static auto inst = std::optional<GFXCMDLogFileView>();

    if (!inst.has_value()) {
        inst = GFXCMDLogFileView();
    }

    return inst.value();
}

}   // namespace gfx::scenery
}   // namespace gfx