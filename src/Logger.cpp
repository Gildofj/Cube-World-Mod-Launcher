#include "Logger.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <windows.h>

namespace fs = std::filesystem;

namespace cw {

namespace {

std::string GetCurrentTimestampFormatted() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t timer = system_clock::to_time_t(now);
    std::tm bt{};
    localtime_s(&bt, &timer);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string_view ExtractFileName(std::string_view fullPath) {
    auto lastSlash = fullPath.find_last_of("/\\");
    if (lastSlash == std::string_view::npos) {
        return fullPath;
    }
    return fullPath.substr(lastSlash + 1);
}

} // anonymous namespace

Logger& Logger::Instance() {
    static Logger s_instance;
    return s_instance;
}

Logger::Logger() = default;

Logger::~Logger() {
    Shutdown();
}

namespace {

std::string GetCurrentTimestampForFilename() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t timer = system_clock::to_time_t(now);
    std::tm bt{};
    localtime_s(&bt, &timer);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

} // anonymous namespace

bool Logger::Init(const std::string& logDir, const std::string& logPrefix, bool echoToConsole) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_logDir = logDir;
    m_echoConsole = echoToConsole;

    std::error_code ec;
    fs::path dirPath(m_logDir);
    if (!fs::exists(dirPath, ec)) {
        fs::create_directories(dirPath, ec);
    }

    std::string timestampFile = GetCurrentTimestampForFilename();
    std::string sessionFileName = logPrefix + "_" + timestampFile + ".log";
    std::string latestFileName = logPrefix + "_latest.log";

    fs::path filePath = dirPath / sessionFileName;
    fs::path latestPath = dirPath / latestFileName;

    m_logFilePath = filePath.string();
    m_latestFilePath = latestPath.string();

    if (m_fileStream.is_open()) {
        m_fileStream.close();
    }
    if (m_latestStream.is_open()) {
        m_latestStream.close();
    }

    // Open session file
    m_fileStream.open(m_logFilePath, std::ios::out | std::ios::trunc);
    // Open latest mirror file
    m_latestStream.open(m_latestFilePath, std::ios::out | std::ios::trunc);

    if (!m_fileStream.is_open()) {
        return false;
    }

    m_initialized = true;

    // Log initialization session header
    std::string timestamp = GetCurrentTimestampFormatted();
    std::string header = "======================================================\n"
                         " Log Session Started: " + timestamp + "\n"
                         " Target Executable: Cube World Mod Loader\n"
                         " Log File: " + m_logFilePath + "\n"
                         "======================================================\n\n";

    m_fileStream << header;
    m_fileStream.flush();

    if (m_latestStream.is_open()) {
        m_latestStream << header;
        m_latestStream.flush();
    }

    return true;
}

void Logger::Shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_fileStream.is_open()) {
        std::string timestamp = GetCurrentTimestampFormatted();
        std::string footer = "\n======================================================\n"
                             " Log Session Ended: " + timestamp + "\n"
                             "======================================================\n";
        m_fileStream << footer;
        m_fileStream.flush();
        m_fileStream.close();

        if (m_latestStream.is_open()) {
            m_latestStream << footer;
            m_latestStream.flush();
            m_latestStream.close();
        }
    }
    m_initialized = false;
}

void Logger::SetLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_minLevel = level;
}

LogLevel Logger::GetLogLevel() const {
    return m_minLevel;
}

const std::string& Logger::GetLogDirectory() const {
    return m_logDir;
}

const std::string& Logger::GetLogFilePath() const {
    return m_logFilePath;
}

const char* Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARN";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

void Logger::Log(LogLevel level, const char* file, int line, const char* format, ...) {
    if (level < m_minLevel) {
        return;
    }

    va_list args;
    va_start(args, format);
    LogV(level, file, line, format, args);
    va_end(args);
}

void Logger::LogV(LogLevel level, const char* file, int line, const char* format, va_list args) {
    if (level < m_minLevel) {
        return;
    }

    va_list argsCopy;
    va_copy(argsCopy, args);
    int size = vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);

    if (size <= 0) {
        return;
    }

    std::vector<char> buffer(static_cast<size_t>(size) + 1);
    vsnprintf(buffer.data(), buffer.size(), format, args);

    std::string message(buffer.data(), size);
    LogFormattedMessage(level, file, line, message);
}

void Logger::LogFormattedMessage(LogLevel level, const char* file, int line, std::string_view message) {
    if (level < m_minLevel) {
        return;
    }

    std::string timestamp = GetCurrentTimestampFormatted();
    DWORD threadId = GetCurrentThreadId();
    std::string_view fileName = file ? ExtractFileName(file) : "unknown";

    std::ostringstream formatted;
    formatted << "[" << timestamp << "] "
              << "[" << LevelToString(level) << "] "
              << "[TID:" << threadId << "] ";

    if (file) {
        formatted << "[" << fileName << ":" << line << "] ";
    }

    formatted << message << "\n";
    std::string formattedStr = formatted.str();

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_echoConsole) {
        if (level >= LogLevel::Error) {
            std::cerr << formattedStr;
        } else {
            std::cout << formattedStr;
        }
    }

    if (m_fileStream.is_open()) {
        m_fileStream << formattedStr;
        m_fileStream.flush();
    }
    if (m_latestStream.is_open()) {
        m_latestStream << formattedStr;
        m_latestStream.flush();
    }
}

void Logger::LogRaw(LogLevel level, std::string_view message) {
    if (level < m_minLevel) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_echoConsole) {
        if (level >= LogLevel::Error) {
            std::cerr << message;
        } else {
            std::cout << message;
        }
    }

    if (m_fileStream.is_open()) {
        m_fileStream << message;
        m_fileStream.flush();
    }
    if (m_latestStream.is_open()) {
        m_latestStream << message;
        m_latestStream.flush();
    }
}

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_fileStream.is_open()) {
        m_fileStream.flush();
    }
    if (m_latestStream.is_open()) {
        m_latestStream.flush();
    }
}

} // namespace cw
