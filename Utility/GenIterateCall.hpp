#ifndef __GenIterateCall
#define __GenIterateCall

#include <iostream>

void genIterateCallMacros(std::ostream& os,
    std::size_t iterationLimit)
{
    if (!iterationLimit) {
        return;
    }

    // header guard open
    os << "#ifndef __MAC_ITERATE_CALL\n";
    os << "#define __MAC_ITERATE_CALL\n";

    // __ITERATE_CALL N(callble, arg1, arg2, arg3, ..., argN)
    // expands to callable(arg1),
    // __ITERATE_CALL N-1(callable, arg2, arg3, arg4, ..., argN)

    os << "#define __ITERATE_CALL1(callable, arg1) callable(arg1)\n";
    
    for (auto idxMacro = std::size_t(2); idxMacro <= iterationLimit;
        ++idxMacro) {
        os << "#define __ITERATE_CALL" << idxMacro << "(callable";
        for (auto idxArg = std::size_t(1); idxArg <= idxMacro; ++idxArg) {
            os << ", arg" << idxArg;
        }
        os << ") callable(arg1), __ITERATE_CALL" << idxMacro - 1
            << "(callable";
        for (auto idxArg = std::size_t(2); idxArg <= idxMacro; ++idxArg) {
            os << ", arg" << idxArg;
        }
        os << ")\n";
    }

    // make ONEHOT_ENCODE(...) call __ONEHOT_ENCODEN

    os << "#define __ITERATE_CALL_SPECIFICATION(callable, N, ...) "
        << "__ITERATE_CALL ## N(callable, __VA_ARGS__)\n";

    // forwarding is necessary to transform
    // a call of a macro into the result of that call.

    os << "#define __ITERATE_CALL_FORWARD_NUMARGS(callable, N, ...) "
        << "__ITERATE_CALL_SPECIFICATION(callable, N, __VA_ARGS__)\n";

    os << "#define ITERATE_CALL(callable, ...) "
        << "__ITERATE_CALL_FORWARD_NUMARGS(callable, "
        << "NUMARGS(__VA_ARGS__), __VA_ARGS__)\n";

    // header guard close
    os << "#endif	// __MAC_ITERATE_CALL";
}

#endif  // __GenIterateCall