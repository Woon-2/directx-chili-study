#ifndef __GFXCMDLogFileView
#define __GFXCMDLogFileView

#include "GFXCMDSummarizer.hpp"

#include <fstream>
#include <filesystem>

#include "Literal.hpp"

#define GFXCMDLOG_FILEVIEW getGFXCMDLogFileView()

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

#endif  // __GFXCMDLogFileView