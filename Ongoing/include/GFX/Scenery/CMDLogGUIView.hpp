#ifndef __GFXCMDLogGUIView
#define __GFXCMDLogGUIView

#include "CMDSummarizer.hpp"

#define GFXCMDLOG_GUIVIEW gfx::scenery::getGFXCMDLogGuiView()

namespace gfx {
namespace scenery {

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

}  // namespace gfx::scenery
}  // namespace gfx

#endif  // __GFXCMDLogGUIView