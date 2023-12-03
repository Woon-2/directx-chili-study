#ifndef __Mouse
#define __Mouse

#include <optional>
#include <queue>

class Mouse {
public:
    friend class MouseMsgAPI;

    struct Point {
        int x;
        int y;
    };

    class Event {
    public:
        enum class Type {
            LPress,
            LRelease,
            RPress,
            RRelease,
            MPress,
            MRelease,
            WheelUp,
            WheelDown,
            Move,
            Enter,
            Leave
        };

        Event() noexcept
            : type_(), pos_{0, 0} {}

        Event(Type typeVal, Point posVal) noexcept
            : type_(typeVal), pos_(posVal) {}

        bool moved() const noexcept {
            return valid()
                && type_.value() == Type::Move;
        }

        bool leftPressed() const noexcept {
            return valid()
                && type_.value() == Type::LPress;
        }

        bool leftReleased() const noexcept {
            return valid()
                && type_.value() == Type::LRelease;
        }

        bool rightPressed() const noexcept {
            return valid()
                && type_.value() == Type::RPress;
        }

        bool rightReleased() const noexcept {
            return valid()
                && type_.value() == Type::RRelease;
        }

        bool midPressed() const noexcept {
            return valid()
                && type_.value() == Type::MPress;
        }

        bool midReleased() const noexcept {
            return valid()
                && type_.value() == Type::MRelease;
        }

        bool WheelUped() const noexcept {
            return valid()
                && type_.value() == Type::WheelUp;
        }

        bool WheelDowned() const noexcept {
            return valid()
                && type_.value() == Type::WheelDown;
        }

        bool entered() const noexcept {
            return valid()
                && type_.value() == Type::Enter;
        }

        bool leaved() const noexcept {
            return valid()
                && type_.value() == Type::Leave;
        }

        bool valid() const noexcept {
            return type_.has_value();
        }

        std::optional<Type> type() const {
            return type_;
        }

        const Point pos() const noexcept {
            return pos_;
        }

    private:
        Point pos_;
        std::optional<Type> type_;
    };

    Mouse(std::size_t bufferSize = 32ull, int wheelThresholdVal = 120)
        : buf_(), bufSize_(bufferSize),
        wheelThreshold_(wheelThresholdVal), wheelDeltaCarry_(),
        bInWindow_(false) {};

    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;
    
    const std::optional<Event> read() noexcept {
        if (auto ret = peek()) {
            buf_.pop();
            return ret;
        }

        return {};
    }

    const std::optional<Event> peek() const noexcept {
        if (empty()) {
            return {};
        }

        return buf_.front();
    }

    void setBufSize(std::size_t bufferSize) noexcept {
        bufSize_ = bufferSize;
    }

    std::size_t bufSize() const noexcept {
        return bufSize_;
    }

    bool empty() const noexcept {
        return buf_.empty();
    }

    void flush() noexcept {
        buf_ = decltype(buf_)();
    }

    void setWheelThreshold(int threshold) noexcept {
        wheelThreshold_ = threshold;
    }

    int wheelThreshold() const noexcept {
        return wheelThreshold_;
    }

    bool inWindow() const noexcept {
        return bInWindow_;
    }

private:
    void onMouseMove(Point pos) {
        buf_.emplace( Event::Type::Move, pos );
        trimBuf();
    }

    void onLeftPressed(Point pos) {
        buf_.emplace( Event::Type::LPress, pos );
        trimBuf();
    }

    void onLeftReleased(Point pos) {
        buf_.emplace( Event::Type::LRelease, pos );
        trimBuf();
    }

    void onRightPressed(Point pos) {
        buf_.emplace( Event::Type::RPress, pos );
        trimBuf();
    }

    void onRightReleased(Point pos) {
        buf_.emplace( Event::Type::RRelease, pos );
        trimBuf();
    }

    void onMidPressed(Point pos) {
        buf_.emplace( Event::Type::MPress, pos );
        trimBuf();
    }

    void onMidReleased(Point pos) {
        buf_.emplace( Event::Type::MRelease, pos );
        trimBuf();
    }

    void onMouseWheel(Point pos, int wheelDelta) {
        wheelDeltaCarry_ += wheelDelta;
        
        auto nWheelStep = static_cast<int>(
            wheelDeltaCarry_ / wheelThreshold()
        );

        wheelDeltaCarry_ -= nWheelStep * wheelThreshold();

        while (nWheelStep > 0) {
            buf_.emplace( Event::Type::WheelUp, pos );
            --nWheelStep;
        }

        while (nWheelStep < 0) {
            buf_.emplace( Event::Type::WheelDown, pos );
            ++nWheelStep;
        }

        trimBuf();
    }

    void onEnter(Point pos) {
        buf_.emplace( Event::Type::Enter, pos );
        trimBuf();
        bInWindow_ = true;
    }

    void onLeave(Point pos) {
        buf_.emplace( Event::Type::Leave, pos );
        trimBuf();
        bInWindow_ = false;
    }

    void trimBuf() noexcept {
        while (buf_.size() > bufSize()) {
            buf_.pop();
        }
    }

    std::queue<Event> buf_;
    std::size_t bufSize_;
    int wheelThreshold_;
    int wheelDeltaCarry_;
    bool bInWindow_;
};

class MouseMsgAPI {
    template <class Wnd>
    friend class MouseMsgHandler;

private:
    static void onMouseMove(Mouse& mouse, Mouse::Point pos) {
        mouse.onMouseMove(pos);
    }

    static void onLeftPressed(Mouse& mouse, Mouse::Point pos) {
        mouse.onLeftPressed(pos);
    }

    static void onLeftReleased(Mouse& mouse, Mouse::Point pos) {
        mouse.onLeftReleased(pos);
    }

    static void onRightPressed(Mouse& mouse, Mouse::Point pos) {
        mouse.onRightPressed(pos);
    }

    static void onRightReleased(Mouse& mouse, Mouse::Point pos) {
        mouse.onRightReleased(pos);
    }

    static void onMidPressed(Mouse& mouse, Mouse::Point pos) {
        mouse.onMidPressed(pos);
    }

    static void onMidReleased(Mouse& mouse, Mouse::Point pos) {
        mouse.onMidReleased(pos);
    }

    static void onMouseWheel(
        Mouse& mouse, Mouse::Point pos, int wheelDelta
    ) {
        mouse.onMouseWheel(pos, wheelDelta);
    }

    static void onEnter(Mouse& mouse, Mouse::Point pos) {
        mouse.onEnter(pos);
    }

    static void onLeave(Mouse& mouse, Mouse::Point pos) {
        mouse.onLeave(pos);
    }
};

#endif  // __Mouse