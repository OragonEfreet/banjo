#include "test.h"

#include <banjo/byte_stream.h>
#include <banjo/memory.h>

TEST_CASE(default_init_gives_nil_stream) {
    bj_byte_stream* stream = bj_new(byte_stream, default, 0);
    REQUIRE_NIL(bj_byte_stream, stream);
    bj_del(byte_stream, stream);
}

TEST_CASE(default_init_with_size_creates_internal_buffer) {
    bj_byte_stream* stream = bj_new(byte_stream, default, 1);
    REQUIRE_VALUE(stream->p_data);
    REQUIRE_EQ(stream->len, 1);
    bj_del(byte_stream, stream);
}

TEST_CASE(init_sets_position_to_0) {
    bj_byte_stream* stream = bj_new(byte_stream, default, 1);
    REQUIRE_EQ(stream->position, 0);
    bj_del(byte_stream, stream);
}

TEST_CASE(init_with_size_created_data_owned_by_the_object) {
    bj_byte_stream* stream = bj_new(byte_stream, default, 1);
    REQUIRE(stream->weak == false);
    bj_del(byte_stream, stream);
}

TEST_CASE(init_from_buffer_contains_pointer_to_the_buffer) {
    u8 buffer;
    bj_byte_stream* stream = bj_new(byte_stream, read, &buffer, 1);
    REQUIRE_EQ(stream->p_data, &buffer);
    bj_del(byte_stream, stream);
}

TEST_CASE(init_from_buffer_but_zero_size_creates_nil_object) {
    u8 buffer;
    bj_byte_stream* stream = bj_new(byte_stream, read, &buffer, 0);
    REQUIRE_NIL(bj_byte_stream, stream);
    bj_del(byte_stream, stream);
}

TEST_CASE(init_from_null_buffer_and_non_zero_size_creates_nil_object) {
    bj_byte_stream* stream = bj_new(byte_stream, read, 0, 1);
    REQUIRE_NIL(bj_byte_stream, stream);
    bj_del(byte_stream, stream);
}

TEST_CASE(init_from_null_buffer_creates_nil_object) {
    bj_byte_stream* stream = bj_new(byte_stream, read, 0, 0);
    REQUIRE_NIL(bj_byte_stream, stream);
    bj_del(byte_stream, stream);
}

TEST_CASE(reading_from_nil_object_returns_0) {
    u8 buffer;
    bj_byte_stream* stream = bj_new(byte_stream, default, 0);
    usize read = bj_byte_stream_read(stream, u8, &buffer);
    REQUIRE_EQ(read, 0);
    bj_del(byte_stream, stream);
}

TEST_CASE(reading_from_nil_object_does_not_change_the_destination_buffer) {
    u8 buffer = 42;
    bj_byte_stream* stream = bj_new(byte_stream, default, 0);
    bj_byte_stream_read(stream, u8, &buffer);
    REQUIRE_EQ(buffer, 42);
    bj_del(byte_stream, stream);
}

TEST_CASE(reading_from_end_returns_0) {
    u8 buffer;
    bj_byte_stream* stream = bj_new(byte_stream, default, 1);
    stream->position = 4;
    usize read = bj_byte_stream_read(stream, u8, &buffer);
    REQUIRE_EQ(read, 0);
    bj_del(byte_stream, stream);
}

TEST_CASE(reading_from_end_does_not_change_the_destination_buffer) {
    u8 buffer = 42;
    bj_byte_stream* stream = bj_new(byte_stream, default, 1);
    stream->position = 4;
    bj_byte_stream_read(stream, u8, &buffer);
    REQUIRE_EQ(buffer, 42);
    bj_del(byte_stream, stream);
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

    END_TESTS();
}

