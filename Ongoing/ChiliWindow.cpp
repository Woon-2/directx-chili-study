#include "ChiliWindow.hpp"

template struct Win32::ChiliWindowTraits<CHAR>;
template struct Win32::ChiliWindowTraits<WCHAR>;

template class Win32::Window< Win32::ChiliWindowTraits<CHAR> >;
template class Win32::Window< Win32::ChiliWindowTraits<WCHAR> >;