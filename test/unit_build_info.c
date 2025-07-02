#include <banjo/api.h>
#include "config.h"

#include "test.h"

#ifdef BJ_BUILD_RELEASE
#   define INFO_EXPECT_DEBUG BJ_FALSE
#else
#   define INFO_EXPECT_DEBUG BJ_TRUE
#endif

#if BJ_HAS_CONFIG(LOG_COLOR)
#   define INFO_EXPECT_COLOR_LOG BJ_TRUE
#else
#   define INFO_EXPECT_COLOR_LOG BJ_FALSE
#endif

TEST_CASE(get_build_info) {
    const bj_build_info* info = bj_get_build_info();
    int cmp = bj_strcmp(info->p_name, BJ_NAME);
    REQUIRE_EQ(cmp, 0);
    REQUIRE_EQ(info->version, BJ_VERSION);
    REQUIRE_EQ(info->debug, INFO_EXPECT_DEBUG);
    REQUIRE_EQ(info->config_log_color, INFO_EXPECT_COLOR_LOG);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);
    RUN_TEST(get_build_info);
    END_TESTS();
}
