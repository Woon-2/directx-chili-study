#ifndef __Pointers
#define __Pointers

#include <concepts>

#include "Woon2Exception.hpp"
#include <sstream>

#define BAD_BORPTR_ACCESS bad_bor_ptr_access(__LINE__, __FILE__)

class bad_bor_ptr_access : public Woon2Exception {
public:
    bad_bor_ptr_access(int lineNum, const char* fileStr) noexcept
        : Woon2Exception(lineNum, fileStr) {}

    const char* what() const noexcept override {
        auto oss = std::ostringstream();

        oss << this->type() << '\n';
        oss << "[Description]\n"
            "tried to access invalidated bor_ptr.\n"
            "bor_ptr becomes invalid state when it has been"
            "default constructed or copied or moved.\n";
        oss << this->metaStr() << '\n';

        this->whatBuffer_ = oss.str();
        return this->whatBuffer_.c_str();
    }
};

// It stands for borrowed pointer.
template <class T>
class bor_ptr {
public:
    bor_ptr() noexcept
        : bor_ptr(nullptr) {}

    bor_ptr(std::nullptr_t) noexcept
        : data_(nullptr) {}

    bor_ptr(T* ptr) noexcept
        : data_(ptr) {}

    bor_ptr(const bor_ptr&) noexcept
        : data_(nullptr) {}

    bor_ptr& operator=(const bor_ptr&) noexcept {
        reset();
        return *this;
    }

    T* get() const noexcept {
        if (!data_) {
            throw BAD_BORPTR_ACCESS;
        }

        return data_;
    }

    void reset(T* ptr) noexcept {
        data_ = ptr;
    }

    void reset(std::nullptr_t) noexcept {
        data_ = nullptr;
    }

    void reset() noexcept {
        reset(nullptr);
    }

    bool valid() const noexcept {
        return static_cast<bool>(data_);
    }

    operator T*() const noexcept {
        return get();
    }

    T& operator*() const noexcept {
        return *get();
    }

    T* operator->() const noexcept {
        return get();
    }

private:
    T* data_;
};

#undef BAD_BORPTR_ACCESS

#endif  // __Pointers