#include "../framework/test_framework.h"
#include "../../CubeModLoader/mutex.h"
#include <thread>
#include <vector>
#include <atomic>

TEST_CASE(MutexSynchronization, BasicLockUnlock) {
    mutex mtx;
    ASSERT_NO_THROW({
        mtx.lock();
        mtx.unlock();
    });
}

TEST_CASE(MutexSynchronization, SequentialLocks) {
    mutex mtx;
    for (int i = 0; i < 100; ++i) {
        mtx.lock();
        mtx.unlock();
    }
    ASSERT_TRUE(true);
}

TEST_CASE(MutexSynchronization, MultiThreadedContentionCounter) {
    mutex mtx;
    long long counter = 0;
    const int NUM_THREADS = 8;
    const int ITERATIONS_PER_THREAD = 10000;

    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);

    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < ITERATIONS_PER_THREAD; ++i) {
                mtx.lock();
                counter++;
                mtx.unlock();
            }
        });
    }

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    ASSERT_EQ(counter, (long long)(NUM_THREADS * ITERATIONS_PER_THREAD));
}

TEST_CASE(MutexSynchronization, ThreadSafeFlagInitialization) {
    mutex mtx;
    bool already_initialized = false;
    int initialization_count = 0;

    const int NUM_THREADS = 16;
    std::vector<std::thread> threads;

    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([&]() {
            mtx.lock();
            if (!already_initialized) {
                // Critical section initialization work
                initialization_count++;
                already_initialized = true;
            }
            mtx.unlock();
        });
    }

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    ASSERT_EQ(initialization_count, 1);
    ASSERT_TRUE(already_initialized);
}
