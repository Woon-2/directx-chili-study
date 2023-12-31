#ifndef __GFXCMDLogGUIView
#define __GFXCMDLogGUIView

#include "GFXCMDSummarizer.hpp"

#define GFXCMDLOG_GUIVIEW getGFXCMDLogGuiView()

class GFXCMDLogGuiView {
public:
    using MyChar = std::filesystem::path::value_type;
    using MyString = std::basic_string<MyChar>;
    using MyOFStream = std::basic_ofstream<MyChar>;

    GFXCMDLogGuiView();

    void render();

private:
    std::size_t nFrameSample_;
    std::size_t frameID_;
    bool willShow_;
};

GFXCMDLogGuiView& getGFXCMDLogGuiView();

#endif  // __GFXCMDLogGUIView