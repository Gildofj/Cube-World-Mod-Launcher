#include "../framework/test_framework.h"
#include <string>

namespace VersionChecker {
    const int LOADER_MAJOR = 7;
    const int LOADER_MINOR = 3;

    enum class CompatibilityResult {
        Compatible,
        ModLoaderTooOld_Major,
        ModAuthorNeedsUpdate_Major,
        ModLoaderTooOld_Minor
    };

    CompatibilityResult CheckCompatibility(int mod_major, int mod_minor) {
        if (mod_major > LOADER_MAJOR) {
            return CompatibilityResult::ModLoaderTooOld_Major;
        }
        if (mod_major < LOADER_MAJOR) {
            return CompatibilityResult::ModAuthorNeedsUpdate_Major;
        }
        if (mod_minor > LOADER_MINOR) {
            return CompatibilityResult::ModLoaderTooOld_Minor;
        }
        return CompatibilityResult::Compatible;
    }
}

TEST_CASE(VersionCompatibility, ExactVersionMatch) {
    auto res = VersionChecker::CheckCompatibility(7, 3);
    ASSERT_TRUE(res == VersionChecker::CompatibilityResult::Compatible);
}

TEST_CASE(VersionCompatibility, MinorBackwardCompatibility) {
    auto res_v0 = VersionChecker::CheckCompatibility(7, 0);
    ASSERT_TRUE(res_v0 == VersionChecker::CompatibilityResult::Compatible);

    auto res_v1 = VersionChecker::CheckCompatibility(7, 1);
    ASSERT_TRUE(res_v1 == VersionChecker::CompatibilityResult::Compatible);

    auto res_v2 = VersionChecker::CheckCompatibility(7, 2);
    ASSERT_TRUE(res_v2 == VersionChecker::CompatibilityResult::Compatible);
}

TEST_CASE(VersionCompatibility, NewerMinorVersionRejection) {
    auto res_v4 = VersionChecker::CheckCompatibility(7, 4);
    ASSERT_TRUE(res_v4 == VersionChecker::CompatibilityResult::ModLoaderTooOld_Minor);

    auto res_v10 = VersionChecker::CheckCompatibility(7, 10);
    ASSERT_TRUE(res_v10 == VersionChecker::CompatibilityResult::ModLoaderTooOld_Minor);
}

TEST_CASE(VersionCompatibility, OlderMajorVersionRejection) {
    auto res_v6 = VersionChecker::CheckCompatibility(6, 99);
    ASSERT_TRUE(res_v6 == VersionChecker::CompatibilityResult::ModAuthorNeedsUpdate_Major);

    auto res_v1 = VersionChecker::CheckCompatibility(1, 0);
    ASSERT_TRUE(res_v1 == VersionChecker::CompatibilityResult::ModAuthorNeedsUpdate_Major);
}

TEST_CASE(VersionCompatibility, NewerMajorVersionRejection) {
    auto res_v8 = VersionChecker::CheckCompatibility(8, 0);
    ASSERT_TRUE(res_v8 == VersionChecker::CompatibilityResult::ModLoaderTooOld_Major);

    auto res_v9 = VersionChecker::CheckCompatibility(9, 1);
    ASSERT_TRUE(res_v9 == VersionChecker::CompatibilityResult::ModLoaderTooOld_Major);
}

TEST_CASE(VersionCompatibility, NegativeOrZeroVersions) {
    auto res_zero = VersionChecker::CheckCompatibility(0, 0);
    ASSERT_TRUE(res_zero == VersionChecker::CompatibilityResult::ModAuthorNeedsUpdate_Major);

    auto res_neg = VersionChecker::CheckCompatibility(-1, 0);
    ASSERT_TRUE(res_neg == VersionChecker::CompatibilityResult::ModAuthorNeedsUpdate_Major);
}
