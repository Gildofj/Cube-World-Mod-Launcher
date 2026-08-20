#include "mutex.h"

mutex::mutex() {
    InitializeCriticalSection(&cs);
}

mutex::~mutex() {
    DeleteCriticalSection(&cs);
}

void mutex::lock() {
    EnterCriticalSection(&cs);
}

void mutex::unlock() {
    LeaveCriticalSection(&cs);
}

bool mutex::try_lock() {
    return TryEnterCriticalSection(&cs) != 0;
}
