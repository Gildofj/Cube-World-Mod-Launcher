#include "../framework/test_framework.h"
#include "../mocks/MockEnvironment.h"
#include "../../CubeModLoader/Logger.h"
#include <fstream>
#include <thread>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE(LoggerSubsystem, InitializationAndDirectoryCreation) {
    Mocking::TempDirectoryScope temp_dir;
    std::string log_dir = (temp_dir.path / "cube-world-logs-test").string();

    bool initOk = cw::Logger::Instance().Init(log_dir, "test", false);
    ASSERT_TRUE(initOk);

    ASSERT_TRUE(fs::exists(log_dir));
    ASSERT_TRUE(fs::exists(cw::Logger::Instance().GetLogFilePath()));
    ASSERT_TRUE(fs::exists(fs::path(log_dir) / "test_latest.log"));

    cw::Logger::Instance().Shutdown();
}

TEST_CASE(LoggerSubsystem, LogLevelFiltering) {
    Mocking::TempDirectoryScope temp_dir;
    std::string log_dir = (temp_dir.path / "cube-world-logs-test").string();
    cw::Logger::Instance().Init(log_dir, "filter", false);

    cw::Logger::Instance().SetLogLevel(cw::LogLevel::Warning);
    ASSERT_EQ(static_cast<int>(cw::Logger::Instance().GetLogLevel()), static_cast<int>(cw::LogLevel::Warning));

    CW_LOG_DEBUG("This debug message should be skipped: %d", 123);
    CW_LOG_INFO("This info message should be skipped: %s", "hello");
    CW_LOG_WARN("This warning message should appear: %s", "active");
    CW_LOG_ERROR("This error message should appear: %d", 404);

    cw::Logger::Instance().Flush();
    std::string filePath = cw::Logger::Instance().GetLogFilePath();
    cw::Logger::Instance().Shutdown();

    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    ASSERT_TRUE(content.find("This warning message should appear: active") != std::string::npos);
    ASSERT_TRUE(content.find("This error message should appear: 404") != std::string::npos);
    ASSERT_TRUE(content.find("This debug message should be skipped") == std::string::npos);
    ASSERT_TRUE(content.find("This info message should be skipped") == std::string::npos);
}

TEST_CASE(LoggerSubsystem, MultiThreadedLogging) {
    Mocking::TempDirectoryScope temp_dir;
    std::string log_dir = (temp_dir.path / "cube-world-logs-test").string();
    cw::Logger::Instance().Init(log_dir, "multithread", false);
    cw::Logger::Instance().SetLogLevel(cw::LogLevel::Debug);

    constexpr int kThreads = 8;
    constexpr int kLogsPerThread = 50;

    std::vector<std::thread> workers;
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([t, kLogsPerThread]() {
            for (int i = 0; i < kLogsPerThread; ++i) {
                CW_LOG_INFO("Thread %d logging entry %d", t, i);
            }
        });
    }

    for (auto& w : workers) {
        w.join();
    }

    std::string filePath = cw::Logger::Instance().GetLogFilePath();
    cw::Logger::Instance().Shutdown();

    std::ifstream file(filePath);
    std::string line;
    int logCount = 0;
    while (std::getline(file, line)) {
        if (line.find("logging entry") != std::string::npos) {
            logCount++;
        }
    }

    ASSERT_EQ(logCount, kThreads * kLogsPerThread);
}

TEST_CASE(LoggerSubsystem, LevelToStringConversions) {
    ASSERT_STREQ(cw::Logger::LevelToString(cw::LogLevel::Debug), "DEBUG");
    ASSERT_STREQ(cw::Logger::LevelToString(cw::LogLevel::Info), "INFO");
    ASSERT_STREQ(cw::Logger::LevelToString(cw::LogLevel::Warning), "WARN");
    ASSERT_STREQ(cw::Logger::LevelToString(cw::LogLevel::Error), "ERROR");
    ASSERT_STREQ(cw::Logger::LevelToString(cw::LogLevel::Critical), "CRITICAL");
}

TEST_CASE(LoggerSubsystem, ModernCpp20FormatLogging) {
    Mocking::TempDirectoryScope temp_dir;
    std::string log_dir = (temp_dir.path / "cube-world-logs-test").string();
    cw::Logger::Instance().Init(log_dir, "fmt_test", false);
    cw::Logger::Instance().SetLogLevel(cw::LogLevel::Debug);

    int level = 99;
    std::string playerName = "CubeHero";
    CW_LOG_FMT_INFO("Player {} advanced to level {}", playerName, level);
    CW_LOG_FMT_WARN("Memory threshold exceeded by {} bytes", 4096);

    cw::Logger::Instance().Flush();
    std::string filePath = cw::Logger::Instance().GetLogFilePath();
    cw::Logger::Instance().Shutdown();

    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    ASSERT_TRUE(content.find("Player CubeHero advanced to level 99") != std::string::npos);
    ASSERT_TRUE(content.find("Memory threshold exceeded by 4096 bytes") != std::string::npos);
}
