#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <mutex>
#include <cstdarg>
#include <format>

namespace cw {

enum class LogLevel {
    Debug = 0,
    Info,
    Warning,
    Error,
    Critical
};

class Logger {
public:
    static Logger& Instance();

    // Initialize the logger directory with a session-based timestamped filename and latest alias
    bool Init(const std::string& logDir = "cube-world-logs", 
              const std::string& logPrefix = "modloader", 
              bool echoToConsole = true);

    // Shutdown and flush all buffers
    void Shutdown();

    // Core log method with variable arguments
    void Log(LogLevel level, const char* file, int line, const char* format, ...);

    // Formatted log message directly with va_list
    void LogV(LogLevel level, const char* file, int line, const char* format, va_list args);

    // Modern C++20 type-safe formatted log
    template<typename... Args>
    void LogFmt(LogLevel level, const char* file, int line, std::format_string<Args...> fmt, Args&&... args) {
        if (level < m_minLevel) return;
        std::string formatted = std::format(fmt, std::forward<Args>(args)...);
        LogFormattedMessage(level, file, line, formatted);
    }

    // Internal helper to record pre-formatted string
    void LogFormattedMessage(LogLevel level, const char* file, int line, std::string_view message);

    // Raw message log (without automatic header prefixing if needed)
    void LogRaw(LogLevel level, std::string_view message);

    // Flush stream to disk
    void Flush();

    // Set minimum active log level
    void SetLogLevel(LogLevel level);
    LogLevel GetLogLevel() const;

    // Get current logs directory path
    const std::string& GetLogDirectory() const;

    // Get active session log file full path
    const std::string& GetLogFilePath() const;

    // Convert level to string
    static const char* LevelToString(LogLevel level);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string m_logDir;
    std::string m_logFilePath;
    std::string m_latestFilePath;
    std::ofstream m_fileStream;
    std::ofstream m_latestStream;
    std::mutex m_mutex;
    LogLevel m_minLevel{LogLevel::Debug};
    bool m_echoConsole{true};
    bool m_initialized{false};
};

} // namespace cw

// Convenience macros
#define CW_LOG_DEBUG(format, ...) cw::Logger::Instance().Log(cw::LogLevel::Debug, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define CW_LOG_INFO(format, ...)  cw::Logger::Instance().Log(cw::LogLevel::Info,  __FILE__, __LINE__, format, ##__VA_ARGS__)
#define CW_LOG_WARN(format, ...)  cw::Logger::Instance().Log(cw::LogLevel::Warning, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define CW_LOG_ERROR(format, ...) cw::Logger::Instance().Log(cw::LogLevel::Error, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define CW_LOG_CRITICAL(format, ...) cw::Logger::Instance().Log(cw::LogLevel::Critical, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define CW_LOG_FMT_DEBUG(fmt, ...) cw::Logger::Instance().LogFmt(cw::LogLevel::Debug, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CW_LOG_FMT_INFO(fmt, ...)  cw::Logger::Instance().LogFmt(cw::LogLevel::Info,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CW_LOG_FMT_WARN(fmt, ...)  cw::Logger::Instance().LogFmt(cw::LogLevel::Warning, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CW_LOG_FMT_ERROR(fmt, ...) cw::Logger::Instance().LogFmt(cw::LogLevel::Error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CW_LOG_FMT_CRITICAL(fmt, ...) cw::Logger::Instance().LogFmt(cw::LogLevel::Critical, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
