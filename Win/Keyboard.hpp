#ifndef __Keyboard
#define __Keyboard

#include <ranges>
#include <bitset>
#include <set>
#include <type_traits>
#include <algorithm>
#include <queue>
#include <optional>

class Keyboard
{
public:
    using KeyType = unsigned char;

    enum class KeyState {
        Pressed,
        Released,
        Invalid
    };

    /*
    Event Class' definition is here for complete type issue,
    Keyboard Class has a EventQueue type member,
    so to obtain Keyboard object's size, the compiler needs to know
    EventQueue object's size.
    this is why EventQueue Class' definition is in Keyboard Class,
    and this is why Event Class' definition is in Keyboard Class.
    */ 
    class Event
    {
    public:
        explicit Event() noexcept;
        explicit Event(KeyType key, KeyState state) noexcept;
        Event(const Event&) noexcept = default;
        Event& operator=(const Event&) noexcept = default;
        Event(Event&&) noexcept = default;
        Event& operator=(Event&&) noexcept = default;

        KeyType getKey() const noexcept;
        KeyState getState() const noexcept;

    private:
        KeyType key_;
        KeyState state_;
    };
private:
    /*
    EventQueue Class' definition is here for complete type issue,
    Keyboard Class has a EventQueue type member,
    so to obtain Keyboard object's size, the compiler needs to know
    EventQueue object's size.
    this is why EventQueue Class' definition is in Keyboard Class,
    */ 
    class EventQueue
    {
    private:
        static constexpr size_t bufferSize = 16;

    public:
        EventQueue(bool bEnable = true);
        EventQueue(const EventQueue&) = default;
        EventQueue& operator=(const EventQueue&) = default;
        EventQueue(EventQueue&&) noexcept = default;
        EventQueue& operator=(EventQueue&&) noexcept = default;

        std::optional<Event> readKey();
        std::optional<KeyType> readChar();
        bool keyEmpty() const;
        bool charEmpty() const;
        void flushKey();
        void flushChar();
        void flush();
        void enableAutoRepeat() noexcept;
        void disableAutoRepeat() noexcept;
        bool autoRepeatEnabled() const noexcept;
        template <class Buffer>
        static void trimBuffer(Buffer& buffer);
        static void trimBuffer(...);
        void onKeyPressed(KeyType key);
        void onKeyReleased(KeyType key);
        void onChar(KeyType ch);
        void enable();
        void disable();
        bool enabled() const;

    private:
        bool bActivate_;
        bool bAutoRepeat_;
        std::queue<Event> keyQueue_;
        std::queue<KeyType> charQueue_;
    };

public:
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
    EventQueue& getEventQueue() noexcept;
    const EventQueue& getEventQueue() const noexcept;

private:
    std::bitset< std::numeric_limits<KeyType>::max() > keys_;
    std::set<KeyType> listenKeys_;
    bool bListen_;
    EventQueue eventQueue_;
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

template <class Buffer>
void Keyboard::EventQueue::trimBuffer(Buffer& buffer)
{
    while (buffer.size() > bufferSize) {
        buffer.pop();
    }
}

void Keyboard::EventQueue::trimBuffer(...)
{
    // this shouldn't compile.
}

#endif  // __Keyboard