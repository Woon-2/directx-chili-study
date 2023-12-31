#include "Game/GFXCMDLogGUIView.hpp"

#include "imgui.h"

#include <optional>

GFXCMDLogGuiView::GFXCMDLogGuiView()
    : nFrameSample_(0u), frameID_(0), willShow_(true) {}

void GFXCMDLogGuiView::update() {
    GFXCMDSUM.update(GFXCMDSUM.phIDFrame, GFXCMDSummarizer::IDFrame(frameID_));
    GFXCMDSUM.update(GFXCMDSUM.phNFrameSample, 1u);

    GFXCMDSUM.update(GFXCMDSUM.phTotalCreateCnt);
    GFXCMDSUM.update(GFXCMDSUM.phTotalBindCnt);
    GFXCMDSUM.update(GFXCMDSUM.phTotalDrawCnt);
}

void GFXCMDLogGuiView::render() {
    update();

    if ( willShow_ && ImGui::Begin( "Graphics Report", &willShow_ ) ) {
        auto curFrameReport = GFXCMDSUM.report(
            "Frame: {}\n"
            "[GPU Commands]\n"
            "    Create: {}, Bind: {}, Draw: {}\n",
            GFXCMDSUM.phIDFrame,
            GFXCMDSUM.phTotalCreateCnt,
            GFXCMDSUM.phTotalBindCnt,
            GFXCMDSUM.phTotalDrawCnt
        );

        ImGui::Text( curFrameReport.c_str() );

        ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)",
            1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate
        );

        ImGui::End();
    }
}

GFXCMDLogGuiView& getGFXCMDLogGuiView() {
    static auto inst = std::optional<GFXCMDLogGuiView>();

    if (!inst.has_value()) {
        inst = GFXCMDLogGuiView();
    }

    return inst.value();
}