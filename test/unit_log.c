#include "test.h"
#include <banjo/log.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Level String Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(log_level_string_trace) {
    const char* s = bj_get_log_level_string(BJ_LOG_TRACE);
    REQUIRE_VALUE(s);
    REQUIRE(strlen(s) > 0);
}

TEST_CASE(log_level_string_debug) {
    const char* s = bj_get_log_level_string(BJ_LOG_DEBUG);
    REQUIRE_VALUE(s);
    REQUIRE(strlen(s) > 0);
}

TEST_CASE(log_level_string_info) {
    const char* s = bj_get_log_level_string(BJ_LOG_INFO);
    REQUIRE_VALUE(s);
    REQUIRE(strlen(s) > 0);
}

TEST_CASE(log_level_string_warn) {
    const char* s = bj_get_log_level_string(BJ_LOG_WARN);
    REQUIRE_VALUE(s);
    REQUIRE(strlen(s) > 0);
}

TEST_CASE(log_level_string_error) {
    const char* s = bj_get_log_level_string(BJ_LOG_ERROR);
    REQUIRE_VALUE(s);
    REQUIRE(strlen(s) > 0);
}

TEST_CASE(log_level_string_fatal) {
    const char* s = bj_get_log_level_string(BJ_LOG_FATAL);
    REQUIRE_VALUE(s);
    REQUIRE(strlen(s) > 0);
}

TEST_CASE(log_level_strings_are_unique) {
    const char* trace = bj_get_log_level_string(BJ_LOG_TRACE);
    const char* debug = bj_get_log_level_string(BJ_LOG_DEBUG);
    const char* info  = bj_get_log_level_string(BJ_LOG_INFO);
    const char* warn  = bj_get_log_level_string(BJ_LOG_WARN);
    const char* error = bj_get_log_level_string(BJ_LOG_ERROR);
    const char* fatal = bj_get_log_level_string(BJ_LOG_FATAL);

    // All should be different
    REQUIRE(strcmp(trace, debug) != 0);
    REQUIRE(strcmp(debug, info) != 0);
    REQUIRE(strcmp(info, warn) != 0);
    REQUIRE(strcmp(warn, error) != 0);
    REQUIRE(strcmp(error, fatal) != 0);
}

////////////////////////////////////////////////////////////////////////////////
// Level Control Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(log_set_and_get_level) {
    int original = bj_get_log_level();

    bj_set_log_level(BJ_LOG_WARN);
    REQUIRE_EQ(bj_get_log_level(), BJ_LOG_WARN);

    bj_set_log_level(BJ_LOG_ERROR);
    REQUIRE_EQ(bj_get_log_level(), BJ_LOG_ERROR);

    bj_set_log_level(BJ_LOG_TRACE);
    REQUIRE_EQ(bj_get_log_level(), BJ_LOG_TRACE);

    // Restore
    bj_set_log_level(original);
}

TEST_CASE(log_level_enum_values_are_ordered) {
    // Levels should be ordered from most verbose to least
    REQUIRE(BJ_LOG_TRACE < BJ_LOG_DEBUG);
    REQUIRE(BJ_LOG_DEBUG < BJ_LOG_INFO);
    REQUIRE(BJ_LOG_INFO < BJ_LOG_WARN);
    REQUIRE(BJ_LOG_WARN < BJ_LOG_ERROR);
    REQUIRE(BJ_LOG_ERROR < BJ_LOG_FATAL);
}

////////////////////////////////////////////////////////////////////////////////
// Log Message Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(log_message_returns_nonzero_for_active_level) {
    int original = bj_get_log_level();
    bj_set_log_level(BJ_LOG_TRACE); // Accept all levels

    // Messages at or above the level should return > 0
    size_t r = bj_log_message(BJ_LOG_INFO, 0, 0, "test message");
    REQUIRE(r > 0);

    bj_set_log_level(original);
}

TEST_CASE(log_message_returns_zero_for_filtered_level) {
    int original = bj_get_log_level();
    bj_set_log_level(BJ_LOG_ERROR); // Only ERROR and FATAL

    // Lower levels should be filtered (return 0)
    size_t r = bj_log_message(BJ_LOG_DEBUG, 0, 0, "filtered");
    REQUIRE_EQ(r, 0);

    bj_set_log_level(original);
}

TEST_CASE(log_message_with_format_args) {
    int original = bj_get_log_level();
    bj_set_log_level(BJ_LOG_TRACE);

    // Should not crash with format arguments
    size_t r = bj_log_message(BJ_LOG_INFO, 0, 0, "value: %d, string: %s", 42, "test");
    REQUIRE(r > 0);

    bj_set_log_level(original);
}

TEST_CASE(log_message_with_file_and_line) {
    int original = bj_get_log_level();
    bj_set_log_level(BJ_LOG_TRACE);

    // Should handle file/line info
    size_t r = bj_log_message(BJ_LOG_INFO, "test_file.c", 123, "message");
    REQUIRE(r > 0);

    bj_set_log_level(original);
}

TEST_CASE(log_message_with_null_file) {
    int original = bj_get_log_level();
    bj_set_log_level(BJ_LOG_TRACE);

    // NULL file should be handled
    size_t r = bj_log_message(BJ_LOG_INFO, 0, 0, "message");
    REQUIRE(r > 0);

    bj_set_log_level(original);
}

////////////////////////////////////////////////////////////////////////////////
// Macro Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(log_macros_compile) {
    int original = bj_get_log_level();
    bj_set_log_level(BJ_LOG_FATAL + 1); // Suppress all output

    // Just verify these compile and don't crash
    bj_trace("trace %d", 1);
    bj_debug("debug %d", 2);
    bj_info("info %d", 3);
    bj_warn("warn %d", 4);
    bj_err("error %d", 5);
    bj_fatal("fatal %d", 6);

    bj_set_log_level(original);
    REQUIRE(1); // Got here without crash
}

////////////////////////////////////////////////////////////////////////////////
// Edge Cases
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(log_empty_message) {
    int original = bj_get_log_level();
    bj_set_log_level(BJ_LOG_TRACE);

    // Empty message should not crash
    size_t r = bj_log_message(BJ_LOG_INFO, 0, 0, "");
    // May or may not output anything, but shouldn't crash
    (void)r;

    bj_set_log_level(original);
    REQUIRE(1);
}

TEST_CASE(log_very_long_message_truncates) {
    int original = bj_get_log_level();
    bj_set_log_level(BJ_LOG_TRACE);

    // Create a message longer than BJ_MAXIMUM_LOG_LEN
    char long_msg[256];
    for (int i = 0; i < 255; ++i) long_msg[i] = 'A';
    long_msg[255] = '\0';

    // Should handle truncation without crashing
    size_t r = bj_log_message(BJ_LOG_INFO, 0, 0, "%s", long_msg);
    // Result should be capped at BJ_MAXIMUM_LOG_LEN
    REQUIRE(r <= BJ_MAXIMUM_LOG_LEN);

    bj_set_log_level(original);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Level strings
    RUN_TEST(log_level_string_trace);
    RUN_TEST(log_level_string_debug);
    RUN_TEST(log_level_string_info);
    RUN_TEST(log_level_string_warn);
    RUN_TEST(log_level_string_error);
    RUN_TEST(log_level_string_fatal);
    RUN_TEST(log_level_strings_are_unique);

    // Level control
    RUN_TEST(log_set_and_get_level);
    RUN_TEST(log_level_enum_values_are_ordered);

    // Log message
    RUN_TEST(log_message_returns_nonzero_for_active_level);
    RUN_TEST(log_message_returns_zero_for_filtered_level);
    RUN_TEST(log_message_with_format_args);
    RUN_TEST(log_message_with_file_and_line);
    RUN_TEST(log_message_with_null_file);

    // Macros
    RUN_TEST(log_macros_compile);

    // Edge cases
    RUN_TEST(log_empty_message);
    RUN_TEST(log_very_long_message_truncates);

    END_TESTS();
}
