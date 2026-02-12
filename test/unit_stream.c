#include "test.h"
#include <banjo/stream.h>
#include <banjo/memory.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Memory Stream Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(stream_read_from_buffer_returns_correct_data) {
    const char data[] = "Hello, World!";
    struct bj_stream* s = bj_open_stream_read(data, sizeof(data) - 1);
    REQUIRE_VALUE(s);

    char buf[32] = {0};
    size_t read = bj_read_stream(s, buf, 5);

    REQUIRE_EQ(read, 5);
    REQUIRE(strncmp(buf, "Hello", 5) == 0);

    bj_close_stream(s);
}

TEST_CASE(stream_read_past_end_returns_partial) {
    const char data[] = "Short";
    struct bj_stream* s = bj_open_stream_read(data, 5);
    REQUIRE_VALUE(s);

    char buf[32] = {0};
    size_t read = bj_read_stream(s, buf, 100); // Request more than available

    REQUIRE_EQ(read, 5);
    REQUIRE(strncmp(buf, "Short", 5) == 0);

    bj_close_stream(s);
}

TEST_CASE(stream_read_entire_buffer) {
    const char data[] = "Complete";
    struct bj_stream* s = bj_open_stream_read(data, 8);
    REQUIRE_VALUE(s);

    char buf[32] = {0};
    size_t read = bj_read_stream(s, buf, 8);

    REQUIRE_EQ(read, 8);
    REQUIRE(memcmp(buf, data, 8) == 0);

    bj_close_stream(s);
}

TEST_CASE(stream_multiple_reads) {
    const char data[] = "ABCDEF";
    struct bj_stream* s = bj_open_stream_read(data, 6);
    REQUIRE_VALUE(s);

    char buf[8] = {0};

    size_t r1 = bj_read_stream(s, buf, 2);
    REQUIRE_EQ(r1, 2);
    REQUIRE(buf[0] == 'A' && buf[1] == 'B');

    size_t r2 = bj_read_stream(s, buf, 2);
    REQUIRE_EQ(r2, 2);
    REQUIRE(buf[0] == 'C' && buf[1] == 'D');

    size_t r3 = bj_read_stream(s, buf, 2);
    REQUIRE_EQ(r3, 2);
    REQUIRE(buf[0] == 'E' && buf[1] == 'F');

    // No more data
    size_t r4 = bj_read_stream(s, buf, 2);
    REQUIRE_EQ(r4, 0);

    bj_close_stream(s);
}

////////////////////////////////////////////////////////////////////////////////
// Seek Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(stream_seek_begin) {
    const char data[] = "0123456789";
    struct bj_stream* s = bj_open_stream_read(data, 10);
    REQUIRE_VALUE(s);

    // Read first 3
    char buf[4] = {0};
    bj_read_stream(s, buf, 3);

    // Seek back to beginning
    size_t pos = bj_seek_stream(s, 0, BJ_SEEK_BEGIN);
    REQUIRE_EQ(pos, 0);

    // Read should start from beginning again
    bj_read_stream(s, buf, 3);
    REQUIRE(strncmp(buf, "012", 3) == 0);

    bj_close_stream(s);
}

TEST_CASE(stream_seek_current) {
    const char data[] = "0123456789";
    struct bj_stream* s = bj_open_stream_read(data, 10);
    REQUIRE_VALUE(s);

    // Seek forward 5 from current (start)
    size_t pos = bj_seek_stream(s, 5, BJ_SEEK_CURRENT);
    REQUIRE_EQ(pos, 5);

    char buf[4] = {0};
    bj_read_stream(s, buf, 3);
    REQUIRE(strncmp(buf, "567", 3) == 0);

    bj_close_stream(s);
}

TEST_CASE(stream_seek_end) {
    const char data[] = "0123456789";
    struct bj_stream* s = bj_open_stream_read(data, 10);
    REQUIRE_VALUE(s);

    // Seek to 3 bytes before end
    size_t pos = bj_seek_stream(s, -3, BJ_SEEK_END);
    REQUIRE_EQ(pos, 7);

    char buf[4] = {0};
    bj_read_stream(s, buf, 3);
    REQUIRE(strncmp(buf, "789", 3) == 0);

    bj_close_stream(s);
}

TEST_CASE(stream_seek_and_tell_are_consistent) {
    const char data[] = "TestData";
    struct bj_stream* s = bj_open_stream_read(data, 8);
    REQUIRE_VALUE(s);

    REQUIRE_EQ(bj_tell_stream(s), 0);

    bj_seek_stream(s, 4, BJ_SEEK_BEGIN);
    REQUIRE_EQ(bj_tell_stream(s), 4);

    bj_seek_stream(s, 2, BJ_SEEK_CURRENT);
    REQUIRE_EQ(bj_tell_stream(s), 6);

    bj_seek_stream(s, -1, BJ_SEEK_END);
    REQUIRE_EQ(bj_tell_stream(s), 7);

    bj_close_stream(s);
}

TEST_CASE(stream_seek_clamps_to_bounds) {
    const char data[] = "Data";
    struct bj_stream* s = bj_open_stream_read(data, 4);
    REQUIRE_VALUE(s);

    // Seek past end should clamp
    size_t pos1 = bj_seek_stream(s, 100, BJ_SEEK_BEGIN);
    REQUIRE_EQ(pos1, 4); // Clamped to length

    // Seek before beginning should clamp
    size_t pos2 = bj_seek_stream(s, -100, BJ_SEEK_BEGIN);
    REQUIRE_EQ(pos2, 0); // Clamped to 0

    bj_close_stream(s);
}

////////////////////////////////////////////////////////////////////////////////
// Length Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(stream_length_correct) {
    const char data[] = "Hello";
    struct bj_stream* s = bj_open_stream_read(data, 5);
    REQUIRE_VALUE(s);

    REQUIRE_EQ(bj_get_stream_length(s), 5);

    bj_close_stream(s);
}

TEST_CASE(stream_empty_buffer) {
    struct bj_stream* s = bj_open_stream_read("", 0);
    REQUIRE_VALUE(s);

    REQUIRE_EQ(bj_get_stream_length(s), 0);

    char buf[4];
    size_t read = bj_read_stream(s, buf, 4);
    REQUIRE_EQ(read, 0);

    bj_close_stream(s);
}

////////////////////////////////////////////////////////////////////////////////
// Edge Cases
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(stream_read_zero_bytes_succeeds) {
    const char data[] = "Data";
    struct bj_stream* s = bj_open_stream_read(data, 4);
    REQUIRE_VALUE(s);

    char buf[4];
    size_t read = bj_read_stream(s, buf, 0);
    REQUIRE_EQ(read, 0);
    REQUIRE_EQ(bj_tell_stream(s), 0); // Position unchanged

    bj_close_stream(s);
}

TEST_CASE(stream_allocate_and_close) {
    struct bj_stream* s = bj_allocate_stream();
    REQUIRE_VALUE(s);

    // Just allocate and free - shouldn't crash
    bj_close_stream(s);
    REQUIRE(1);
}

TEST_CASE(stream_close_null_is_safe) {
    bj_close_stream(0);
    REQUIRE(1); // No crash
}

////////////////////////////////////////////////////////////////////////////////
// File Stream Tests (if file exists)
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(stream_open_nonexistent_file_returns_error) {
    struct bj_error* err = 0;
    struct bj_stream* s = bj_open_stream_file("/nonexistent/path/file.txt", &err);

    REQUIRE_NULL(s);
    REQUIRE_VALUE(err);

    bj_clear_error(&err);
}

TEST_CASE(stream_open_file_null_error_is_safe) {
    // Opening non-existent file with NULL error should not crash
    struct bj_stream* s = bj_open_stream_file("/nonexistent/path/file.txt", 0);
    REQUIRE_NULL(s);
}

////////////////////////////////////////////////////////////////////////////////
// Macro Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(stream_read_t_macro) {
    uint32_t data[] = {0x12345678, 0xDEADBEEF};
    struct bj_stream* s = bj_open_stream_read(data, sizeof(data));
    REQUIRE_VALUE(s);

    uint32_t val = 0;
    bj_stream_read_t(s, uint32_t, &val);
    REQUIRE_EQ(val, 0x12345678);

    bj_stream_read_t(s, uint32_t, &val);
    REQUIRE_EQ(val, 0xDEADBEEF);

    bj_close_stream(s);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Memory stream
    RUN_TEST(stream_read_from_buffer_returns_correct_data);
    RUN_TEST(stream_read_past_end_returns_partial);
    RUN_TEST(stream_read_entire_buffer);
    RUN_TEST(stream_multiple_reads);

    // Seek
    RUN_TEST(stream_seek_begin);
    RUN_TEST(stream_seek_current);
    RUN_TEST(stream_seek_end);
    RUN_TEST(stream_seek_and_tell_are_consistent);
    RUN_TEST(stream_seek_clamps_to_bounds);

    // Length
    RUN_TEST(stream_length_correct);
    RUN_TEST(stream_empty_buffer);

    // Edge cases
    RUN_TEST(stream_read_zero_bytes_succeeds);
    RUN_TEST(stream_allocate_and_close);
    RUN_TEST(stream_close_null_is_safe);

    // File stream
    RUN_TEST(stream_open_nonexistent_file_returns_error);
    RUN_TEST(stream_open_file_null_error_is_safe);

    // Macros
    RUN_TEST(stream_read_t_macro);

    END_TESTS();
}
