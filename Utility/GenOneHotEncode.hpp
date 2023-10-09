#ifndef __GenOneHotEncode
#define __GenOneHotEncode

#include <iostream>
#include <string_view>

void genOneHotEncodeMacros(std::ostream& os, std::size_t bitWidth,
    std::string_view suffix)
{
    if (!bitWidth) {
        return;
    }

    // header guard open
	os << "#ifndef __MAC_ONEHOT_ENCODE\n";
	os << "#define __MAC_ONEHOT_ENCODE\n";
    
    // only __ONEOT_ENCODE1 does real encoding,
    // other __ONEHOT_ENCODENs are just
    // recursive permutation of __oneHotEncode1.
    
    os << "#define __ONEHOT_ENCODE1(x, bitPlace) "
        << "x = 1" << suffix << " << bitPlace\n";
    
    // __ONEHOT_ENCODEN concatenates
    // __ONEHOT_ENCODEN-1 and __ONEHOT_ENCODE1.
    
    for (auto idxMacro = std::size_t(2); idxMacro <= bitWidth;
        ++idxMacro)
    {
        os << "#define __ONEHOT_ENCODE" << idxMacro << '(';
        for (size_t idxArg = 1; idxArg <= idxMacro; ++idxArg) {
            os << 'x' << idxArg << ", ";
        }
        os << "bitPlace)\t\\\n";

        os << "\t__ONEHOT_ENCODE" << idxMacro - 1 << '(';
        for (size_t idxArg = 1; idxArg < idxMacro; ++idxArg)
        {
            os << 'x' << idxArg << ", ";
        }
        os << "bitPlace),\t\\\n";

        os << "\t__ONEHOT_ENCODE1(x"
            << idxMacro << ", (bitPlace + "
            << idxMacro - 1 << suffix << "))\n";
    }
    
    // make ONEHOT_ENCODE(...) call __ONEHOT_ENCODEN
    
    os << "#define __ONEHOT_ENCODE_SPECIFICATION(N, ...) "
        << "__ONEHOT_ENCODE ## N(__VA_ARGS__, 0"
        << suffix << ")\n";

    // forwarding is necessary to transform
    // a call of a macro into the result of that call.
        
    os << "#define __ONEHOT_ENCODE_FORWARD_NUMARGS"
        << "(N, ...) __ONEHOT_ENCODE_SPECIFICATION"
        << "(N, __VA_ARGS__)\n";
        
    os << "#define ONEHOT_ENCODE(...) "
        << "__ONEHOT_ENCODE_FORWARD_NUMARGS("
        << "NUMARGS(__VA_ARGS__), "
        << "__VA_ARGS__)\n";

    // header guard close
	os << "#endif	// __MAC_ONEHOT_ENCODE";
}

#endif  // __GenOneHotEncode