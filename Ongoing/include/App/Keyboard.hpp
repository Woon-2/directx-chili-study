#ifndef __Keyboard
#define __Keyboard

#include "ChiliWindow.hpp"

#include <type_traits>
#include <bitset>
#include <queue>
#include <optional>

template <class CharT>
class Keyboard {
public:
    using MyChar = CharT;
    using VK = unsigned char;
    template <class CharU>
    friend class KeyboardMsgAPI;

    class Event {
    public:
        enum class Type {
            Press,
            Release
        };

        Event() noexcept
            : type_(), keycode_(0u) {}

        Event(Type typeVal, VK keycodeVal) noexcept
            : type_(typeVal), keycode_(keycodeVal) {}

        bool pressed() const noexcept {
            return valid()
                && type_.value() == Type::Press;
        }

        bool released() const noexcept {
            return valid()
                && type_.value() == Type::Release;
        }

        bool valid() const noexcept {
            return type_.has_value();
        }

        std::optional<Type> type() const {
            return type_;
        }

        VK keycode() const noexcept {
            return keycode_;
        }

    private:
        std::optional<Type> type_;
        VK keycode_;
    };

    Keyboard(std::size_t keyBufferSize = 16ull,
        std::size_t charBufferSize = 16ull
    ) noexcept
        : bAutoRepeat_(false), scanned_(),
        keyBuf_(), keyBufSize_(keyBufferSize),
        charBuf_(), charBufSize_(charBufferSize) {};

    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;

    bool pressed(VK keycode) const noexcept {
        return scanned_.test(keycode);
    }

    std::optional<Event> readKey() noexcept {
        if (auto ret = peekKey()) {
            keyBuf_.pop();
            return ret;
        }

        return {};
    }

    std::optional<Event> peekKey() const noexcept {
        if (emptyKey()) {
            return {};
        }

        return keyBuf_.front();
    }

    bool emptyKey() const noexcept {
        return keyBuf_.empty();
    }

    void flushKey() noexcept {
        keyBuf_ = decltype(keyBuf_)();
    }

    std::optional<MyChar> readChar() noexcept {
        if (auto ret = peekChar()) {
            charBuf_.pop();
            return ret;
        }

        return {};
    }

    std::optional<MyChar> peekChar() const noexcept {
        if (emptyChar()) {
            return {};
        }

        return charBuf_.front();
    }

    bool emptyChar() const noexcept {
        return charBuf_.empty();
    }

    void flushChar() noexcept {
        charBuf_ = decltype(charBuf_)();
    }

    void flush() noexcept {
        flushKey();
        flushChar();
    }

    void enableAutoRepeat() noexcept {
        bAutoRepeat_ = true;
    }

    void disableAutoRepeat() noexcept {
        bAutoRepeat_ = false;
    }

    bool autoRepeat() const noexcept {
        return bAutoRepeat_;
    }

    void setKeyBufSize(std::size_t size) {
        keyBufSize_ = size;
    }

    std::size_t keyBufSize() const noexcept {
        return keyBufSize_;
    }

    void setCharBufSize(std::size_t size) {
        charBufSize_ = size;
    }

    std::size_t charBufSize() const noexcept {
        return charBufSize_;
    }

private:
    static constexpr auto nKeys = 256u;

    void onKeyPressed(VK keycode) noexcept {
        scanned_.set(keycode);
        keyBuf_.emplace( Event::Type::Press, keycode );
        trimKeyBuf();
    }

    void onKeyReleased(VK keycode) noexcept {
        scanned_.reset(keycode);
        keyBuf_.emplace( Event::Type::Release, keycode );
        trimKeyBuf();
    }

    void onChar(MyChar ch) noexcept {
        charBuf_.push(ch);
        trimCharBuf();
    }

    void clearScanned() noexcept {
        scanned_.reset();
    }

    void trimKeyBuf() noexcept {
        while (keyBuf_.size() > keyBufSize()) {
            keyBuf_.pop();
        }
    }

    void trimCharBuf() noexcept {
        while (charBuf_.size() > charBufSize()) {
            charBuf_.pop();
        }
    }

    bool bAutoRepeat_;
    std::bitset<nKeys> scanned_;
    std::queue<Event> keyBuf_;
    std::size_t keyBufSize_;
    std::queue<MyChar> charBuf_;
    std::size_t charBufSize_;
};

template <class CharT>
class KeyboardMsgAPI {
public:
    using MyChar = CharT;
    using VK = unsigned char;

    template <class Wnd>
    friend class KbdMsgHandler;

private:
    static void onKeyPressed(Keyboard<MyChar>& kbd, VK keycode) {
        kbd.onKeyPressed(keycode);
    }

    static void onKeyReleased(Keyboard<MyChar>& kbd, VK keycode) {
        kbd.onKeyReleased(keycode);
    }

    static void onChar(Keyboard<MyChar>& kbd, MyChar ch) {
        kbd.onChar(ch);
    }

    static void clearScanned(Keyboard<MyChar>& kbd) {
        kbd.clearScanned();
    }
};

#endif  // __Keyboard