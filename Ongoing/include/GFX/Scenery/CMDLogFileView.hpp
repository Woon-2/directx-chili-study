#ifndef __GFXCMDLogFileView
#define __GFXCMDLogFileView

#include "CMDSummarizer.hpp"

#include <fstream>
#include <filesystem>

#include "Literal.hpp"

#define GFXCMDLOG_FILEVIEW gfx::scenery::getGFXCMDLogFileView()

namespace gfx {
namespace scenery {

class GFXCMDLogFileView {
public:
    using MyChar = std::filesystem::path::value_type;
    using MyString = std::basic_string<MyChar>;
    using MyOFStream = std::basic_ofstream<MyChar>;

    GFXCMDLogFileView()
        : out_( __LITERAL(MyChar, "GFXCMDLOG.txt") ) {}

    GFXCMDLogFileView(const std::filesystem::path& path)
        : out_(path) {}

    void report();

private:
    MyOFStream out_;
};

GFXCMDLogFileView& getGFXCMDLogFileView();

}  // namespace gfx::scenery
}  // namespace gfx

#endif  // __GFXCMDLogFileView