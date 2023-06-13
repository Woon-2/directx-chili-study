#include "Keyboard.hpp"

#include <Windows.h>

Keyboard::Keyboard(bool bListen) noexcept
    : keys_(0u), listenKeys_(), bListen_(bListen), eventQueue_()
{}

Keyboard::Keyboard(std::set<Keyboard::KeyType>&& listenKeys,
    bool bListen) noexcept
    : keys_(0u), listenKeys_( std::move(listenKeys) ), bListen_(bListen),
    eventQueue_()
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

Keyboard::EventQueue& Keyboard::getEventQueue() noexcept
{
    return eventQueue_;
}

const Keyboard::EventQueue& Keyboard::getEventQueue() const noexcept
{
    return eventQueue_;
}

Keyboard::Event::Event() noexcept
    : key_(), state_(KeyState::Invalid)
{}

Keyboard::Event::Event(KeyType key, KeyState state) noexcept
    : key_(key), state_(state)
{}

Keyboard::KeyType Keyboard::Event::getKey() const noexcept
{
    return key_;
}

Keyboard::KeyState Keyboard::Event::getState() const noexcept
{
    return state_;
}

Keyboard::EventQueue::EventQueue(bool bEnable)
    : bActivate_(bEnable), bAutoRepeat_(false), keyQueue_(),
    charQueue_()
{}

std::optional<Keyboard::Event> Keyboard::EventQueue::readKey()
{
    if (keyQueue_.size()) {
        auto ev = keyQueue_.front();
        keyQueue_.pop();
        return ev;
    }
    else {
        return std::nullopt;
    }
}

std::optional<Keyboard::KeyType> Keyboard::EventQueue::readChar()
{
    if (charQueue_.size()) {
        auto ch = charQueue_.front();
        charQueue_.pop();
        return ch;
    }
    else {
        return std::nullopt;
    }
}

bool Keyboard::EventQueue::keyEmpty() const
{
    return false;
}

bool Keyboard::EventQueue::charEmpty() const
{
    return false;
}

void Keyboard::EventQueue::flushKey()
{
    keyQueue_ = decltype(keyQueue_)();
}

void Keyboard::EventQueue::flushChar()
{
    charQueue_ = decltype(charQueue_)();
}

void Keyboard::EventQueue::flush()
{
    flushKey();
    flushChar();
}

void Keyboard::EventQueue::enableAutoRepeat() noexcept
{
    bAutoRepeat_ = true;
}

void Keyboard::EventQueue::disableAutoRepeat() noexcept
{
    bAutoRepeat_ = false;
}

bool Keyboard::EventQueue::autoRepeatEnabled() const noexcept
{
    return bAutoRepeat_;
}

void Keyboard::EventQueue::onKeyPressed(KeyType key)
{
    if ( !enabled() ) {
        return;
    }
    keyQueue_.push( Event(key, KeyState::Pressed) );
    trimBuffer(keyQueue_);
}

void Keyboard::EventQueue::onKeyReleased(KeyType key)
{
    if ( !enabled() ) {
        return;
    }
    keyQueue_.push( Event(key, KeyState::Released) );
    trimBuffer(keyQueue_);
}

void Keyboard::EventQueue::onChar(KeyType ch)
{
    if ( !enabled() ) {
        return;
    }
    charQueue_.push(ch);
    trimBuffer(charQueue_);
}

void Keyboard::EventQueue::enable()
{
    bActivate_ = true;
}

void Keyboard::EventQueue::disable()
{
    bActivate_ = false;
    flush();
}

bool Keyboard::EventQueue::enabled() const
{
    return bActivate_;
}

void Keyboard::EventQueue::trimBuffer(...)
{
    static_assert("trimBuffer must support push & pop methods.");
}