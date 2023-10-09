#ifndef __GenNumArgs
#define __GenNumArgs

#include <iostream>

void genNumArgsMacros(std::ostream& os, std::size_t n)
{
    if (!n) {
        return;
    }

    // header guard open
	os << "#ifndef __MAC_NUMARGS\n";
	os << "#define __MAC_NUMARGS\n";

    // __EXTRACT_NUMARGS arranges number of varadic args + n
    // arguments in descending order.
    // __EXTRACT_NUMARGS fetches the n + 1th element,
    // so the fetched value is equal to n.

    os << "#define __EXTRACT_NUMARGS(";
    for (auto idxArg = std::size_t(1); idxArg <= n;
        ++idxArg) {
        os << 'x' << idxArg << ", ";
    }
    os << "N, ...) N\n";

    // NUMARGS is the macro actually used.
    // NUMARGS fetches the number of given arguments,
    // with above __EXTRACT_NUMARGS helper macro.
    
    os << "#define NUMARGS(...)\t\\\n";
    os << "\t__EXTRACT_NUMARGS(__VA_ARGS__, ";
    for (auto idxN = n; idxN > 0; --idxN) {
        os << idxN << ", ";
    }
    os << "0)\n";

	// header guard close
	os << "#endif	// __MAC_NUMARGS";
}

#endif  // __GenNumArgs