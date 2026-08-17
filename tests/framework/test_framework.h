#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <chrono>
#include <cmath>
#include <memory>

namespace TestFramework {

    struct TestFailure {
        std::string test_name;
        std::string file;
        int line;
        std::string condition;
        std::string message;
    };

    class TestCase {
    public:
        std::string suite_name;
        std::string test_name;
        std::function<void()> func;

        TestCase(std::string suite, std::string name, std::function<void()> f)
            : suite_name(std::move(suite)), test_name(std::move(name)), func(std::move(f)) {}
    };

    class TestRegistry {
    private:
        std::vector<TestCase> tests;
        std::vector<TestFailure> failures;
        int current_assertions = 0;
        int total_assertions = 0;
        std::string current_suite;
        std::string current_test;

        TestRegistry() = default;

    public:
        static TestRegistry& instance() {
            static TestRegistry reg;
            return reg;
        }

        void register_test(const std::string& suite, const std::string& name, std::function<void()> f) {
            tests.emplace_back(suite, name, f);
        }

        void record_assertion() {
            current_assertions++;
            total_assertions++;
        }

        void record_failure(const std::string& file, int line, const std::string& condition, const std::string& message) {
            failures.push_back({current_suite + "::" + current_test, file, line, condition, message});
        }

        bool has_current_test_failed(size_t failure_count_before) const {
            return failures.size() > failure_count_before;
        }

        int run_all() {
            int passed = 0;
            int failed = 0;
            failures.clear();
            total_assertions = 0;

            std::cout << "\n=================================================================\n";
            std::cout << "  Cube World Mod Launcher - Test Execution Engine\n";
            std::cout << "=================================================================\n\n";

            auto start_time = std::chrono::high_resolution_clock::now();

            std::string last_suite = "";

            for (const auto& test : tests) {
                if (test.suite_name != last_suite) {
                    last_suite = test.suite_name;
                    std::cout << "\n[SUITE] " << last_suite << "\n";
                    std::cout << "-----------------------------------------------------------------\n";
                }

                current_suite = test.suite_name;
                current_test = test.test_name;
                current_assertions = 0;
                size_t failures_before = failures.size();

                auto t0 = std::chrono::high_resolution_clock::now();
                bool threw_exception = false;
                std::string exception_msg = "";

                try {
                    test.func();
                } catch (const std::exception& e) {
                    threw_exception = true;
                    exception_msg = e.what();
                } catch (...) {
                    threw_exception = true;
                    exception_msg = "Unknown non-standard exception thrown";
                }

                auto t1 = std::chrono::high_resolution_clock::now();
                double elapsed_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

                if (threw_exception) {
                    record_failure(__FILE__, __LINE__, "Uncaught Exception", exception_msg);
                }

                if (has_current_test_failed(failures_before)) {
                    failed++;
                    std::cout << "  [FAIL] " << test.test_name << " (" << elapsed_ms << " ms)\n";
                } else {
                    passed++;
                    std::cout << "  [PASS] " << test.test_name << " (" << elapsed_ms << " ms, "
                              << current_assertions << " asserts)\n";
                }
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            double total_elapsed_s = std::chrono::duration<double>(end_time - start_time).count();

            std::cout << "\n=================================================================\n";
            std::cout << "  TEST SUMMARY\n";
            std::cout << "=================================================================\n";
            std::cout << "  Total Tests:       " << tests.size() << "\n";
            std::cout << "  Passed:            " << passed << "\n";
            std::cout << "  Failed:            " << failed << "\n";
            std::cout << "  Total Assertions:  " << total_assertions << "\n";
            std::cout << "  Execution Time:    " << total_elapsed_s << " s\n";

            if (!failures.empty()) {
                std::cout << "\n=================================================================\n";
                std::cout << "  FAILURE DETAILS (" << failures.size() << " failure(s))\n";
                std::cout << "=================================================================\n";
                for (size_t i = 0; i < failures.size(); ++i) {
                    const auto& f = failures[i];
                    std::cout << "\n  #" << (i + 1) << " " << f.test_name << "\n";
                    std::cout << "     File:      " << f.file << ":" << f.line << "\n";
                    std::cout << "     Condition: " << f.condition << "\n";
                    if (!f.message.empty()) {
                        std::cout << "     Message:   " << f.message << "\n";
                    }
                }
                std::cout << "\n";
                return 1;
            }

            std::cout << "\n>>> ALL TESTS PASSED SUCCESSFULLY! <<<\n\n";
            return 0;
        }
    };

    struct AutoRegister {
        AutoRegister(const std::string& suite, const std::string& name, std::function<void()> f) {
            TestRegistry::instance().register_test(suite, name, f);
        }
    };

} // namespace TestFramework

#define TEST_CASE(suite, name) \
    static void _test_##suite##_##name(); \
    static ::TestFramework::AutoRegister _reg_##suite##_##name(#suite, #name, _test_##suite##_##name); \
    static void _test_##suite##_##name()

#define ASSERT_TRUE(cond) \
    do { \
        ::TestFramework::TestRegistry::instance().record_assertion(); \
        if (!(cond)) { \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, #cond, "Expected true, got false"); \
            return; \
        } \
    } while(0)

#define ASSERT_FALSE(cond) \
    do { \
        ::TestFramework::TestRegistry::instance().record_assertion(); \
        if (cond) { \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, #cond, "Expected false, got true"); \
            return; \
        } \
    } while(0)

#define ASSERT_EQ(val1, val2) \
    do { \
        ::TestFramework::TestRegistry::instance().record_assertion(); \
        auto _v1 = (val1); \
        auto _v2 = (val2); \
        if (_v1 != _v2) { \
            std::ostringstream _oss; \
            _oss << "Expected " << _v1 << " == " << _v2; \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, #val1 " == " #val2, _oss.str()); \
            return; \
        } \
    } while(0)

#define ASSERT_NE(val1, val2) \
    do { \
        ::TestFramework::TestRegistry::instance().record_assertion(); \
        auto _v1 = (val1); \
        auto _v2 = (val2); \
        if (_v1 == _v2) { \
            std::ostringstream _oss; \
            _oss << "Expected " << _v1 << " != " << _v2; \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, #val1 " != " #val2, _oss.str()); \
            return; \
        } \
    } while(0)

#define ASSERT_NEAR(val1, val2, tolerance) \
    do { \
        ::TestFramework::TestRegistry::instance().record_assertion(); \
        auto _v1 = (val1); \
        auto _v2 = (val2); \
        if (std::fabs(_v1 - _v2) > (tolerance)) { \
            std::ostringstream _oss; \
            _oss << "Expected |" << _v1 << " - " << _v2 << "| <= " << tolerance << " (diff: " << std::fabs(_v1 - _v2) << ")"; \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, "|" #val1 " - " #val2 "| <= " #tolerance, _oss.str()); \
            return; \
        } \
    } while(0)

#define ASSERT_STREQ(str1, str2) \
    do { \
        ::TestFramework::TestRegistry::instance().record_assertion(); \
        std::string _s1(str1); \
        std::string _s2(str2); \
        if (_s1 != _s2) { \
            std::ostringstream _oss; \
            _oss << "Expected \"" << _s1 << "\" == \"" << _s2 << "\""; \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, #str1 " == " #str2, _oss.str()); \
            return; \
        } \
    } while(0)

#define ASSERT_THROW(expr, ExceptionType) \
    do { \
        ::TestFramework::TestRegistry::instance().record_assertion(); \
        bool _threw_expected = false; \
        try { \
            expr; \
        } catch (const ExceptionType&) { \
            _threw_expected = true; \
        } catch (...) { \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, #expr, "Threw unexpected exception type (not " #ExceptionType ")"); \
            return; \
        } \
        if (!_threw_expected) { \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, #expr, "Expected exception of type " #ExceptionType " was not thrown"); \
            return; \
        } \
    } while(0)

#define ASSERT_NO_THROW(expr) \
    do { \
        ::TestFramework::TestRegistry::instance().record_assertion(); \
        try { \
            expr; \
        } catch (const std::exception& _e) { \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, #expr, std::string("Unexpected exception: ") + _e.what()); \
            return; \
        } catch (...) { \
            ::TestFramework::TestRegistry::instance().record_failure(__FILE__, __LINE__, #expr, "Unexpected non-standard exception thrown"); \
            return; \
        } \
    } while(0)

#endif // TEST_FRAMEWORK_H
