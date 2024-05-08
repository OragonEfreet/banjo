#include "test.h"

#include <banjo/byte_stream.h>
#include <banjo/memory.h>

TEST_CASE(default_init_gives_nil_stream) {

    bj_byte_stream* stream = bj_new(byte_stream, default, 0);


}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(default_init_gives_nil_stream);

    END_TESTS();
}

