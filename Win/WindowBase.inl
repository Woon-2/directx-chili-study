#include "WindowBase.hpp"

#include <sstream>
#include <type_traits>

template <class Concrete, class CharT, class Traits, class Allocator>
void WindowBase<Concrete, CharT, Traits, Allocator>::
injectWndProc(WNDCLASSEXW& wc) noexcept
{
    wc.lpfnWndProc = handleMsgSetup;
}

template <class Concrete, class CharT, class Traits, class Allocator>
void WindowBase<Concrete, CharT, Traits, Allocator>::
injectWndProc(WNDCLASSEXA& wc) noexcept
{
    wc.lpfnWndProc = handleMsgSetup;
}

template <class Concrete, class CharT, class Traits, class Allocator>
LRESULT CALLBACK WindowBase<Concrete, CharT, Traits, Allocator>::
handleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    using MyCreateStruct = std::conditional_t<
        std::is_same_v<CharT, char>, CREATESTRUCTA,
        std::conditional_t<
            std::is_same_v<CharT, wchar_t>, CREATESTRUCTW,
            std::false_type
        >
    >;

    if constexpr( std::is_same_v<MyCreateStruct, std::false_type> ) {
        static_assert("There's no matching character type on Window to CharT.");
    }

    if (msg != WM_NCCREATE) {
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }

    // extract ptr to window from creation data
    const auto pCreate = reinterpret_cast<
        const MyCreateStruct*>( lParam );
    auto pWnd = static_cast<WindowBase*>( pCreate->lpCreateParams );
    
    // make WinAPI-managed user data to store the ptr to window
    SetWindowLongPtr( hWnd, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>( pWnd ) );
    // setup is done,
    // change message handler into regular one.
    SetWindowLongPtr( hWnd, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>( &WindowBase::handleMsgThunk ) );

    return pWnd->handleMsgForward( hWnd, msg, wParam, lParam );
}

template <class Concrete, class CharT, class Traits, class Allocator>
LRESULT CALLBACK WindowBase<Concrete, CharT, Traits, Allocator>::
handleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    auto pWnd = reinterpret_cast< WindowBase* >(
        GetWindowLongPtr( hWnd, GWLP_USERDATA )
    );

    return pWnd->handleMsgForward( hWnd, msg, wParam, lParam );
}

template <class Concrete, class CharT, class Traits, class Allocator>
LRESULT WindowBase<Concrete, CharT, Traits, Allocator>::
handleMsgForward( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    return static_cast<Concrete*>(this)->handleMsg(hWnd, msg, wParam, lParam);
}