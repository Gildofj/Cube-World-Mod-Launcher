#ifndef SCOPED_HANDLE_H
#define SCOPED_HANDLE_H

#include <windows.h>
#include <utility>

/**
 * @brief RAII wrapper for Windows HANDLE to ensure deterministic resource cleanup.
 */
class ScopedHandle {
public:
    ScopedHandle() noexcept : handle_(nullptr) {}

    explicit ScopedHandle(HANDLE handle) noexcept : handle_(handle) {}

    ~ScopedHandle() noexcept {
        reset();
    }

    // Non-copyable
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

    // Moveable
    ScopedHandle(ScopedHandle&& other) noexcept : handle_(other.release()) {}

    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    [[nodiscard]] HANDLE get() const noexcept {
        return handle_;
    }

    [[nodiscard]] bool isValid() const noexcept {
        return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
    }

    explicit operator bool() const noexcept {
        return isValid();
    }

    HANDLE* getAddressOf() noexcept {
        reset();
        return &handle_;
    }

    HANDLE release() noexcept {
        HANDLE temp = handle_;
        handle_ = nullptr;
        return temp;
    }

    void reset(HANDLE newHandle = nullptr) noexcept {
        if (isValid()) {
            CloseHandle(handle_);
        }
        handle_ = newHandle;
    }

private:
    HANDLE handle_;
};

#endif // SCOPED_HANDLE_H
