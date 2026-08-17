#ifndef GCC_MUTEX_H
#define GCC_MUTEX_H

#include <windows.h>

/**
 * @brief Thread synchronization primitive wrapping Windows Critical Section.
 */
class mutex {
public:
    mutex();
    ~mutex();

    // Non-copyable
    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    void lock();
    void unlock();
    bool try_lock();

private:
    CRITICAL_SECTION cs;
};

/**
 * @brief RAII lock guard for mutex.
 */
class ScopedLock {
public:
    explicit ScopedLock(mutex& m) : mtx_(m) {
        mtx_.lock();
    }

    ~ScopedLock() {
        mtx_.unlock();
    }

    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;

private:
    mutex& mtx_;
};

#endif // GCC_MUTEX_H
