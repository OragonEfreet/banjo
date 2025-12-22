#include "test.h"

#include <banjo/memory.h>

#include "stream.h"

TEST_CASE(init_from_buffer_contains_pointer_to_the_buffer) {
    uint8_t buffer = 0;
    struct bj_stream* p_stream = bj_open_stream_read(&buffer, 1);
    REQUIRE_EQ(p_stream->data.r, &buffer);
    bj_close_stream(p_stream);
}

TEST_CASE(init_from_buffer_but_zero_size_returns_valid_object) {
    uint8_t buffer = 0;
    struct bj_stream* p_stream = bj_open_stream_read(&buffer, 0);
    REQUIRE_VALUE(p_stream);
    REQUIRE_EQ(p_stream->data.r, &buffer);
    REQUIRE_EQ(p_stream->len, 0);
    REQUIRE_EQ(p_stream->position, 0);
    bj_close_stream(p_stream);
}

TEST_CASE(init_from_null_buffer_and_non_zero_size_returns_valid_object) {
    struct bj_stream* p_stream = bj_open_stream_read(0, 1);
    REQUIRE_VALUE(p_stream);
    REQUIRE_EQ(p_stream->data.r, 0);
    REQUIRE_EQ(p_stream->len, 1);
    REQUIRE_EQ(p_stream->position, 0);
    bj_close_stream(p_stream);
}

TEST_CASE(init_from_null_buffer_returns_valid) {
    struct bj_stream* p_stream = bj_open_stream_read(0, 0);
    REQUIRE_VALUE(p_stream);
    REQUIRE_EQ(p_stream->data.r, 0);
    REQUIRE_EQ(p_stream->len, 0);
    REQUIRE_EQ(p_stream->position, 0);
    bj_close_stream(p_stream);
}

TEST_CASE(reading_n_bytes_shift_the_position_to_n_bytes) {
    uint8_t src[88] = {0};
    size_t read_sizes[]     = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    size_t pos_after_read[] = {0, 1, 2, 4, 7, 12, 20, 33, 54, 88};

    struct bj_stream* p_stream = bj_open_stream_read(src, 88);

    for(size_t i = 0 ; i < 10 ; ++i) {
        bj_read_stream(p_stream, 0, read_sizes[i]);
        REQUIRE_EQ(p_stream->position, pos_after_read[i]);
    }

    bj_close_stream(p_stream);
}

TEST_CASE(reading_n_bytes_returns_the_number_of_read_bytes) {
    uint8_t buffer[88] = {0};
    size_t read_sizes[]     = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};

    struct bj_stream* p_stream = bj_open_stream_read(buffer, 88);

    for(size_t i = 0 ; i < 10 ; ++i) {
        size_t read = bj_read_stream(p_stream, 0, read_sizes[i]);
        REQUIRE_EQ(read, read_sizes[i]);
    }

    bj_close_stream(p_stream);
}

TEST_CASE(reading_to_get_values) {
    int src[]        = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    int test_value[] = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    int dest         = 100;

    struct bj_stream* p_stream = bj_open_stream_read(src, sizeof(int) * 10);

    for(size_t i = 0 ; i < 10 ; ++i) {
        bj_stream_read_t(p_stream, int, &dest);
        REQUIRE_EQ(dest, test_value[i]);
    }

    bj_close_stream(p_stream);
}

TEST_CASE(reading_n_bytes_shift_tell_return_to_n_bytes) {
    uint8_t src[88] = {0};
    size_t read_sizes[]     = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    size_t pos_after_read[] = {0, 1, 2, 4, 7, 12, 20, 33, 54, 88};

    struct bj_stream* p_stream = bj_open_stream_read(src, 88);

    for(size_t i = 0 ; i < 10 ; ++i) {
        bj_read_stream(p_stream, 0, read_sizes[i]);
        size_t pos = bj_tell_stream(p_stream);
        REQUIRE_EQ(pos, pos_after_read[i]);
    }

    bj_close_stream(p_stream);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(init_from_buffer_but_zero_size_returns_valid_object);
    RUN_TEST(init_from_buffer_contains_pointer_to_the_buffer);
    RUN_TEST(init_from_null_buffer_and_non_zero_size_returns_valid_object);
    RUN_TEST(init_from_null_buffer_returns_valid);
    RUN_TEST(reading_n_bytes_returns_the_number_of_read_bytes);
    RUN_TEST(reading_n_bytes_shift_tell_return_to_n_bytes);
    RUN_TEST(reading_n_bytes_shift_the_position_to_n_bytes);
    RUN_TEST(reading_to_get_values);

    END_TESTS();
}

