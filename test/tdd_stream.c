#include "test.h"

#include <banjo/memory.h>

#include "stream_t.h"

TEST_CASE(init_from_buffer_contains_pointer_to_the_buffer) {
    u8 buffer;
    bj_stream* p_stream = bj_stream_new_read(&buffer, 1);
    REQUIRE_EQ(p_stream->p_data.r, &buffer);
    bj_stream_del(p_stream);
}

TEST_CASE(init_from_buffer_but_zero_size_returns_0) {
    u8 buffer;
    bj_stream* p_stream = bj_stream_new_read(&buffer, 0);
    REQUIRE_NULL(p_stream)
}

TEST_CASE(init_from_null_buffer_and_non_zero_size_returns_0) {
    bj_stream* p_stream = bj_stream_new_read(0, 1);
    REQUIRE_NULL(p_stream)
}

TEST_CASE(init_from_null_buffer_returns_0) {
    bj_stream* p_stream = bj_stream_new_read(0, 0);
    REQUIRE_NULL(p_stream)
}

TEST_CASE(reading_n_bytes_shift_the_position_to_n_bytes) {
    u8 src[88];
    usize read_sizes[]     = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    usize pos_after_read[] = {0, 1, 2, 4, 7, 12, 20, 33, 54, 88};

    bj_stream* p_stream = bj_stream_new_read(src, 88);

    for(usize i = 0 ; i < 10 ; ++i) {
        bj_stream_read(p_stream, 0, read_sizes[i]);
        REQUIRE_EQ(p_stream->position, pos_after_read[i]);
    }

    bj_stream_del(p_stream);
}

TEST_CASE(reading_n_bytes_returns_the_number_of_read_bytes) {
    u8 buffer[88];
    usize read_sizes[]     = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};

    bj_stream* p_stream = bj_stream_new_read(buffer, 88);

    for(usize i = 0 ; i < 10 ; ++i) {
        usize read = bj_stream_read(p_stream, 0, read_sizes[i]);
        REQUIRE_EQ(read, read_sizes[i]);
    }

    bj_stream_del(p_stream);
}

TEST_CASE(reading_to_get_values) {
    int src[]        = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    int test_value[] = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    int dest         = 100;

    bj_stream* p_stream = bj_stream_new_read(src, sizeof(int) * 10);

    for(usize i = 0 ; i < 10 ; ++i) {
        bj_stream_read_t(p_stream, int, &dest);
        REQUIRE_EQ(dest, test_value[i]);
    }

    bj_stream_del(p_stream);
}

TEST_CASE(reading_n_bytes_shift_tell_return_to_n_bytes) {
    u8 src[88];
    usize read_sizes[]     = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    usize pos_after_read[] = {0, 1, 2, 4, 7, 12, 20, 33, 54, 88};

    bj_stream* p_stream = bj_stream_new_read(src, 88);

    for(usize i = 0 ; i < 10 ; ++i) {
        bj_stream_read(p_stream, 0, read_sizes[i]);
        usize pos = bj_stream_tell(p_stream);
        REQUIRE_EQ(pos, pos_after_read[i]);
    }

    bj_stream_del(p_stream);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(init_from_buffer_but_zero_size_returns_0);
    RUN_TEST(init_from_buffer_contains_pointer_to_the_buffer);
    RUN_TEST(init_from_null_buffer_and_non_zero_size_returns_0);
    RUN_TEST(init_from_null_buffer_returns_0);
    RUN_TEST(reading_n_bytes_returns_the_number_of_read_bytes);
    RUN_TEST(reading_n_bytes_shift_tell_return_to_n_bytes);
    RUN_TEST(reading_n_bytes_shift_the_position_to_n_bytes);
    RUN_TEST(reading_to_get_values);

    END_TESTS();
}

