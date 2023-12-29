#include "UI/ImguiMgr.hpp"
#include "imgui.h"

ImguiMgr::ImguiMgr() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
}

ImguiMgr::~ImguiMgr() {
    ImGui::DestroyContext();
}