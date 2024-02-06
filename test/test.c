#include "test.h"

int test(int a) { return 0;}

TEST_CASE(zero_initialization) {
    CHECK(5 == 4);

}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(zero_initialization);

    END_TESTS();
}
