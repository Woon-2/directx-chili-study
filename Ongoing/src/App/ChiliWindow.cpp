#include "App/ChiliWindow.hpp"

#include "UI/ImguiMgr.hpp"

template struct Win32::ChiliWindowTraits<CHAR>;
template struct Win32::ChiliWindowTraits<WCHAR>;

template class Win32::Window< Win32::ChiliWindowTraits<CHAR> >;
template class Win32::Window< Win32::ChiliWindowTraits<WCHAR> >;

template class Win32::BasicChiliWindow< Win32::ChiliWindowTraits<CHAR> >;
template class Win32::BasicChiliWindow< Win32::ChiliWindowTraits<WCHAR> >;

ImguiMgr gImguiMgr;