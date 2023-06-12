#ifndef __Keyboard
#define __Keyboard

#include <ranges>
#include <bitset>
#include <set>
#include <limits>
#include <type_traits>
#include <algorithm>

class Keyboard
{
public:
    using KeyType = unsigned char;

    enum class KeyState {
        Pressed,
        Released,
        Invalid
    };

    explicit Keyboard(bool bListen = true) noexcept;
    template <std::ranges::input_range R>
    explicit constexpr Keyboard(R&& listenKeys, bool bListen = true) noexcept(noexcept(
        std::ranges::range_value_t<R>( std::declval< std::ranges::range_value_t<R> >() )
        ));
    explicit Keyboard(std::set<KeyType>&& listenKeys,
        bool bListen = true) noexcept;

    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;
    Keyboard(Keyboard&&) = delete;
    Keyboard& operator=(Keyboard&&) = delete;

    void scan();
    void listenKey(KeyType key) noexcept;
    void ignoreKey(KeyType key) noexcept;
    KeyState getKeyState(KeyType key) const;
    template <std::ranges::input_range R>
    constexpr void embedListenKeys(R&& listenKeys) noexcept;
    void embedListenKeys(std::set<KeyType>&& listenKeys) noexcept;
    constexpr void enableListening() noexcept;
    constexpr void disableListening() noexcept;

private:
    std::bitset< std::numeric_limits<KeyType>::max() > keys_;
    std::set<KeyType> listenKeys_;
    bool bListen_;
};

template <std::ranges::input_range R>
constexpr Keyboard::Keyboard(R&& listenKeys, bool bListen) noexcept(noexcept(
    std::ranges::range_value_t<R>( std::declval< std::ranges::range_value_t<R> >() )
    ))
    : keys_(0u), listenKeys_(), bListen_(bListen)
{
    embedListenKeys( std::forward<R>(listenKeys) );
}

template <std::ranges::input_range R>
inline constexpr void Keyboard::embedListenKeys(R&& listenKeys) noexcept
{
    std::copy( std::cbegin(listenKeys), std::cend(listenKeys),
        std::back_inserter(listenKeys_) );
}

#endif  // __Keyboard