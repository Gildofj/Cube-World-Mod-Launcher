#include "../framework/test_framework.h"
#include <vector>
#include <string>

namespace PaginationTest {
    const int MODS_PER_PAGE = 7;

    struct FakePaginationWidget {
        int page = 0;
        size_t mod_count = 0;

        bool NextPageAvailable() const {
            return (page + 1) * MODS_PER_PAGE < mod_count;
        }

        bool PreviousPageAvailable() const {
            return page > 0;
        }

        void GoNext() {
            if (NextPageAvailable()) {
                page++;
            }
        }

        void GoPrevious() {
            if (PreviousPageAvailable()) {
                page--;
            }
        }

        int TotalPages() const {
            return static_cast<int>(mod_count / MODS_PER_PAGE) + 1;
        }
    };

    enum HoverState {
        None = 0,
        Exit = 1,
        Next = 2,
        Previous = 3,
        Toggle = 4
    };
}

TEST_CASE(ModWidgetPagination, ZeroMods) {
    PaginationTest::FakePaginationWidget widget;
    widget.page = 0;
    widget.mod_count = 0;

    ASSERT_FALSE(widget.NextPageAvailable());
    ASSERT_FALSE(widget.PreviousPageAvailable());
    ASSERT_EQ(widget.TotalPages(), 1);
}

TEST_CASE(ModWidgetPagination, ExactSinglePageBoundary_7Mods) {
    PaginationTest::FakePaginationWidget widget;
    widget.page = 0;
    widget.mod_count = 7;

    ASSERT_FALSE(widget.NextPageAvailable());
    ASSERT_FALSE(widget.PreviousPageAvailable());
    ASSERT_EQ(widget.TotalPages(), 2);
}

TEST_CASE(ModWidgetPagination, EightMods_TwoPages) {
    PaginationTest::FakePaginationWidget widget;
    widget.page = 0;
    widget.mod_count = 8;

    ASSERT_TRUE(widget.NextPageAvailable());
    ASSERT_FALSE(widget.PreviousPageAvailable());

    widget.GoNext();
    ASSERT_EQ(widget.page, 1);
    ASSERT_FALSE(widget.NextPageAvailable());
    ASSERT_TRUE(widget.PreviousPageAvailable());

    widget.GoPrevious();
    ASSERT_EQ(widget.page, 0);
}

TEST_CASE(ModWidgetPagination, MultiPageNavigation_25Mods) {
    PaginationTest::FakePaginationWidget widget;
    widget.page = 0;
    widget.mod_count = 25; // Pages: 0..6 (p0), 7..13 (p1), 14..20 (p2), 21..24 (p3)

    ASSERT_TRUE(widget.NextPageAvailable());
    widget.GoNext(); // page 1
    ASSERT_EQ(widget.page, 1);
    ASSERT_TRUE(widget.NextPageAvailable());
    ASSERT_TRUE(widget.PreviousPageAvailable());

    widget.GoNext(); // page 2
    ASSERT_EQ(widget.page, 2);
    ASSERT_TRUE(widget.NextPageAvailable());

    widget.GoNext(); // page 3
    ASSERT_EQ(widget.page, 3);
    ASSERT_FALSE(widget.NextPageAvailable());
    ASSERT_TRUE(widget.PreviousPageAvailable());

    // Try to advance past max page
    widget.GoNext();
    ASSERT_EQ(widget.page, 3);

    // Navigate back to beginning
    widget.GoPrevious();
    widget.GoPrevious();
    widget.GoPrevious();
    ASSERT_EQ(widget.page, 0);
    ASSERT_FALSE(widget.PreviousPageAvailable());
}

TEST_CASE(ModWidgetPagination, HoverStatesEnumIntegrity) {
    ASSERT_EQ((int)PaginationTest::HoverState::None, 0);
    ASSERT_EQ((int)PaginationTest::HoverState::Exit, 1);
    ASSERT_EQ((int)PaginationTest::HoverState::Next, 2);
    ASSERT_EQ((int)PaginationTest::HoverState::Previous, 3);
    ASSERT_EQ((int)PaginationTest::HoverState::Toggle, 4);
}
