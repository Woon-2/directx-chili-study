#include "UI/ImguiMgr.hpp"

ImguiMgr::ImguiMgr() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
}

ImguiMgr::~ImguiMgr() {
    ImGui::DestroyContext();
}