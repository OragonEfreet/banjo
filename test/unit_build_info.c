#include <banjo/api.h>
#include <banjo/version.h>

#include "test.h"

#ifdef BJ_BUILD_RELEASE
#   define INFO_EXPECT_DEBUG BJ_FALSE
#else
#   define INFO_EXPECT_DEBUG BJ_TRUE
#endif

#ifdef BJ_CONFIG_LOG_COLOR
#   define INFO_EXPECT_COLOR_LOG BJ_TRUE
#else
#   define INFO_EXPECT_COLOR_LOG BJ_FALSE
#endif

TEST_CASE(get_build_info) {
    const struct bj_build_info* info = bj_build_information();
    int cmp = bj_strcmp(info->name, BJ_NAME);
    REQUIRE_EQ(cmp, 0);
    REQUIRE_EQ(info->version, BJ_VERSION);
    REQUIRE_EQ(info->debug, INFO_EXPECT_DEBUG);
    REQUIRE_EQ(info->log_color, INFO_EXPECT_COLOR_LOG);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);
    RUN_TEST(get_build_info);
    END_TESTS();
}
