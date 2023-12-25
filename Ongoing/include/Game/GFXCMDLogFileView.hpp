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

    void report() {
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

private:
    MyOFStream out_;
};

GFXCMDLogFileView& getGFXCMDLogFileView();

#endif  // __GFXCMDLogFileView