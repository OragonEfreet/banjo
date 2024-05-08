#include "test.h"

#include <banjo/stream.h>
#include <banjo/memory.h>

TEST_CASE(default_init_gives_nil_stream) {
    bj_stream* stream = bj_new(stream, default, 0);
    REQUIRE_NIL(bj_stream, stream);
    bj_del(stream, stream);
}

TEST_CASE(default_init_with_size_creates_internal_buffer) {
    bj_stream* stream = bj_new(stream, default, 1);
    REQUIRE_VALUE(stream->p_data);
    REQUIRE_EQ(stream->len, 1);
    bj_del(stream, stream);
}

TEST_CASE(init_sets_position_to_0) {
    bj_stream* stream = bj_new(stream, default, 1);
    REQUIRE_EQ(stream->position, 0);
    bj_del(stream, stream);
}

TEST_CASE(init_with_size_created_data_owned_by_the_object) {
    bj_stream* stream = bj_new(stream, default, 1);
    REQUIRE(stream->weak == false);
    bj_del(stream, stream);
}

TEST_CASE(init_from_buffer_contains_pointer_to_the_buffer) {
    u8 buffer;
    bj_stream* stream = bj_new(stream, read, &buffer, 1);
    REQUIRE_EQ(stream->p_data, &buffer);
    bj_del(stream, stream);
}

TEST_CASE(init_from_buffer_but_zero_size_creates_nil_object) {
    u8 buffer;
    bj_stream* stream = bj_new(stream, read, &buffer, 0);
    REQUIRE_NIL(bj_stream, stream);
    bj_del(stream, stream);
}

TEST_CASE(init_from_null_buffer_and_non_zero_size_creates_nil_object) {
    bj_stream* stream = bj_new(stream, read, 0, 1);
    REQUIRE_NIL(bj_stream, stream);
    bj_del(stream, stream);
}

TEST_CASE(init_from_null_buffer_creates_nil_object) {
    bj_stream* stream = bj_new(stream, read, 0, 0);
    REQUIRE_NIL(bj_stream, stream);
    bj_del(stream, stream);
}

TEST_CASE(reading_from_nil_object_returns_0) {
    u8 buffer;
    bj_stream* stream = bj_new(stream, default, 0);
    usize read = bj_stream_read(stream, u8, &buffer);
    REQUIRE_EQ(read, 0);
    bj_del(stream, stream);
}

TEST_CASE(reading_from_nil_object_does_not_change_the_destination_buffer) {
    u8 buffer = 42;
    bj_stream* stream = bj_new(stream, default, 0);
    bj_stream_read(stream, u8, &buffer);
    REQUIRE_EQ(buffer, 42);
    bj_del(stream, stream);
}

TEST_CASE(reading_from_end_returns_0) {
    u8 buffer;
    bj_stream* stream = bj_new(stream, default, 1);
    stream->position = 4;
    usize read = bj_stream_read(stream, u8, &buffer);
    REQUIRE_EQ(read, 0);
    bj_del(stream, stream);
}

TEST_CASE(reading_from_end_does_not_change_the_destination_buffer) {
    u8 buffer = 42;
    bj_stream* stream = bj_new(stream, default, 1);
    stream->position = 4;
    bj_stream_read(stream, u8, &buffer);
    REQUIRE_EQ(buffer, 42);
    bj_del(stream, stream);
}

TEST_CASE(reading_n_bytes_shift_the_position_to_n_bytes) {
    u8 buffer[88];
    u8 dest;
    usize read_sizes[]     = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    usize pos_after_read[] = {0, 1, 2, 4, 7, 12, 20, 33, 54, 88};

    bj_stream* stream = bj_new(stream, read, buffer, 88);

    for(usize i = 0 ; i < 10 ; ++i) {
        bj_stream_read_byte(stream, read_sizes[i], &dest);
        REQUIRE_EQ(stream->position, pos_after_read[i]);
    }

    bj_del(stream, stream);
}

TEST_CASE(reading_n_bytes_returns_the_number_of_read_bytes) {
    u8 buffer[88];
    usize read_sizes[]     = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};

    bj_stream* stream = bj_new(stream, read, buffer, 88);

    for(usize i = 0 ; i < 10 ; ++i) {
        usize read = bj_stream_read_byte(stream, read_sizes[i], 0);
        REQUIRE_EQ(read, read_sizes[i]);
    }

    bj_del(stream, stream);
}

TEST_CASE(reading_to_get_values) {
    usize src[] = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    usize test_value[] = {0, 1, 1, 2, 3,  5,  8, 13, 21, 34};
    usize dest = 100;

    bj_stream* stream = bj_new(stream, read, src, 10);

    for(usize i = 0 ; i < 10 ; ++i) {
        bj_stream_read(stream, usize, &dest);
        REQUIRE_EQ(dest, test_value[i]);
    }

    bj_del(stream, stream);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(default_init_gives_nil_stream);
    RUN_TEST(default_init_with_size_creates_internal_buffer);
    RUN_TEST(init_sets_position_to_0);
    RUN_TEST(init_with_size_created_data_owned_by_the_object);
    RUN_TEST(init_from_buffer_contains_pointer_to_the_buffer);
    RUN_TEST(init_from_null_buffer_creates_nil_object);
    RUN_TEST(init_from_buffer_but_zero_size_creates_nil_object);
    RUN_TEST(init_from_null_buffer_and_non_zero_size_creates_nil_object);
    RUN_TEST(reading_from_nil_object_returns_0);
    RUN_TEST(reading_from_nil_object_does_not_change_the_destination_buffer);
    RUN_TEST(reading_from_end_returns_0);
    RUN_TEST(reading_from_end_does_not_change_the_destination_buffer);
    RUN_TEST(reading_n_bytes_shift_the_position_to_n_bytes);
    RUN_TEST(reading_n_bytes_returns_the_number_of_read_bytes);
    RUN_TEST(reading_to_get_values);

    END_TESTS();
}

