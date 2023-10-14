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
            Move
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
        wheelThreshold_(wheelThresholdVal), wheelDelta_() {};

    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;
    
    const std::optional<Event> read() noexcept {
        if (empty()) {
            return {};
        }

        auto ret = buf_.front();
        buf_.pop();
        return ret;
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

        wheelDelta_ += wheelDelta;
        
        auto nWheelStep = static_cast<int>(
            wheelDelta_ / wheelThreshold()
        );

        wheelDelta_ -= nWheelStep * wheelThreshold();

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

    void trimBuf() noexcept {
        while (buf_.size() > bufSize()) {
            buf_.pop();
        }
    }

    std::queue<Event> buf_;
    std::size_t bufSize_;
    int wheelThreshold_;
    int wheelDelta_;
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
};

#endif  // __Mouse