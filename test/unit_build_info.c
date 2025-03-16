#include "banjo/api.h"

#include "test.h"

#include <string.h>

#ifdef BJ_BUILD_RELEASE
#   define INFO_EXPECT_DEBUG false
#else
#   define INFO_EXPECT_DEBUG true
#endif

#if BJ_CONFIG_PEDANTIC
#   define INFO_EXPECT_PEDANTIC true
#else
#   define INFO_EXPECT_PEDANTIC false
#endif

#if BJ_CONFIG_LOG_COLOR
#   define INFO_EXPECT_COLOR_LOG true
#else
#   define INFO_EXPECT_COLOR_LOG false
#endif

TEST_CASE(get_build_info) {
    const bj_build_info* info = bj_get_build_info();
    int cmp = strcmp(info->p_name, BJ_NAME);
    REQUIRE_EQ(cmp, 0);
    REQUIRE_EQ(info->version, BJ_VERSION);
    REQUIRE_EQ(info->debug, INFO_EXPECT_DEBUG);
    REQUIRE_EQ(info->config_pedantic, INFO_EXPECT_PEDANTIC);
    REQUIRE_EQ(info->config_log_color, INFO_EXPECT_COLOR_LOG);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);
    RUN_TEST(get_build_info);
    END_TESTS();
}
