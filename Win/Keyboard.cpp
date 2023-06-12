#include "Keyboard.hpp"

#include <Windows.h>

Keyboard::Keyboard(bool bListen) noexcept
    : keys_(0u), listenKeys_(), bListen_(bListen)
{}

Keyboard::Keyboard(std::set<Keyboard::KeyType>&& listenKeys,
    bool bListen) noexcept
    : keys_(0u), listenKeys_( std::move(listenKeys) ), bListen_(bListen)
{}

void Keyboard::scan()
{
    for (const auto key : listenKeys_) {
        keys_.set( static_cast<size_t>(key),
            static_cast<bool>( GetAsyncKeyState(key) & 0x8000 ) );
    }
}

void Keyboard::listenKey(KeyType key) noexcept
{
    listenKeys_.insert(key);
}

void Keyboard::ignoreKey(KeyType key) noexcept
{
    listenKeys_.erase(key);
}

Keyboard::KeyState Keyboard::getKeyState(KeyType key) const
{
    if ( listenKeys_.find(key) == listenKeys_.end() ) {
        return KeyState::Invalid;
    }
    else if ( keys_.test( static_cast<size_t>(key) ) ) {
        return KeyState::Pressed;
    }
    else {
        return KeyState::Released;
    }
}

void Keyboard::embedListenKeys(std::set<KeyType>&& listenKeys) noexcept
{
    listenKeys_ = std::move(listenKeys);
}

constexpr void Keyboard::enableListening() noexcept
{
    bListen_ = true;
}

constexpr void Keyboard::disableListening() noexcept
{
    bListen_ = false;
}
