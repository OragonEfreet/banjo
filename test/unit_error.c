#include "test.h"
#include <banjo/error.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Creation Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(set_error_stores_code_and_message) {
    struct bj_error* err = 0;
    bj_set_error(&err, BJ_ERROR_FILE_NOT_FOUND, "file.txt not found");

    REQUIRE_VALUE(err);
    REQUIRE_EQ(bj_error_code(err), BJ_ERROR_FILE_NOT_FOUND);
    REQUIRE_VALUE(bj_error_message(err));
    REQUIRE(strcmp(bj_error_message(err), "file.txt not found") == 0);

    bj_clear_error(&err);
    REQUIRE_NULL(err);
}

TEST_CASE(set_error_fmt_formats_correctly) {
    struct bj_error* err = 0;
    bj_set_error_fmt(&err, BJ_ERROR_CANNOT_READ, "Cannot read %d bytes from %s", 42, "stream");

    REQUIRE_VALUE(err);
    REQUIRE_EQ(bj_error_code(err), BJ_ERROR_CANNOT_READ);
    REQUIRE(strstr(bj_error_message(err), "42") != 0);
    REQUIRE(strstr(bj_error_message(err), "stream") != 0);

    bj_clear_error(&err);
}

TEST_CASE(set_error_with_null_destination_is_safe) {
    // Should not crash when error destination is NULL
    bj_set_error(0, BJ_ERROR, "ignored");
    bj_set_error_fmt(0, BJ_ERROR, "ignored %d", 42);
    REQUIRE(1); // If we got here, no crash
}

TEST_CASE(set_error_first_error_wins) {
    struct bj_error* err = 0;
    bj_set_error(&err, BJ_ERROR_FILE_NOT_FOUND, "first error");
    bj_set_error(&err, BJ_ERROR_CANNOT_READ, "second error");

    // First error should be preserved
    REQUIRE_EQ(bj_error_code(err), BJ_ERROR_FILE_NOT_FOUND);
    REQUIRE(strcmp(bj_error_message(err), "first error") == 0);

    bj_clear_error(&err);
}

////////////////////////////////////////////////////////////////////////////////
// Propagation Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(propagate_error_transfers_ownership) {
    struct bj_error* src = 0;
    struct bj_error* dst = 0;

    bj_set_error(&src, BJ_ERROR_IO, "io error");
    bj_propagate_error(&dst, src);

    REQUIRE_VALUE(dst);
    REQUIRE_EQ(bj_error_code(dst), BJ_ERROR_IO);
    // src is consumed, don't use it

    bj_clear_error(&dst);
}

TEST_CASE(propagate_prefixed_error_prepends_message) {
    struct bj_error* src = 0;
    struct bj_error* dst = 0;

    bj_set_error(&src, BJ_ERROR_CANNOT_WRITE, "disk full");
    bj_propagate_prefixed_error(&dst, src, "While saving file: ");

    REQUIRE_VALUE(dst);
    REQUIRE(strstr(bj_error_message(dst), "While saving file:") != 0);
    REQUIRE(strstr(bj_error_message(dst), "disk full") != 0);

    bj_clear_error(&dst);
}

TEST_CASE(propagate_to_null_clears_source) {
    struct bj_error* src = 0;
    bj_set_error(&src, BJ_ERROR, "some error");

    // Propagating to NULL should free the source error
    bj_propagate_error(0, src);
    // src is now consumed, if we got here no crash
    REQUIRE(1);
}

////////////////////////////////////////////////////////////////////////////////
// Inspection Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(error_matches_returns_true_for_exact_code) {
    struct bj_error* err = 0;
    bj_set_error(&err, BJ_ERROR_FILE_NOT_FOUND, "missing");

    REQUIRE_EQ(bj_error_matches(err, BJ_ERROR_FILE_NOT_FOUND), BJ_TRUE);
    REQUIRE_EQ(bj_error_matches(err, BJ_ERROR_CANNOT_READ), BJ_FALSE);

    bj_clear_error(&err);
}

TEST_CASE(error_matches_kind_checks_category) {
    struct bj_error* err = 0;

    // BJ_ERROR_FILE_NOT_FOUND has kind BJ_ERROR_SYSTEM
    bj_set_error(&err, BJ_ERROR_FILE_NOT_FOUND, "missing");
    REQUIRE_EQ(bj_error_matches_kind(err, BJ_ERROR_SYSTEM), BJ_TRUE);
    REQUIRE_EQ(bj_error_matches_kind(err, BJ_ERROR_IO), BJ_FALSE);
    bj_clear_error(&err);

    // BJ_ERROR_CANNOT_READ has kind BJ_ERROR_IO
    bj_set_error(&err, BJ_ERROR_CANNOT_READ, "read fail");
    REQUIRE_EQ(bj_error_matches_kind(err, BJ_ERROR_IO), BJ_TRUE);
    REQUIRE_EQ(bj_error_matches_kind(err, BJ_ERROR_SYSTEM), BJ_FALSE);
    bj_clear_error(&err);
}

TEST_CASE(error_code_on_null_returns_none) {
    REQUIRE_EQ(bj_error_code(0), BJ_ERROR_NONE);
}

TEST_CASE(error_message_on_null_returns_null) {
    REQUIRE_NULL(bj_error_message(0));
}

TEST_CASE(error_matches_on_null_returns_false) {
    REQUIRE_EQ(bj_error_matches(0, BJ_ERROR), BJ_FALSE);
    REQUIRE_EQ(bj_error_matches_kind(0, BJ_ERROR), BJ_FALSE);
}

////////////////////////////////////////////////////////////////////////////////
// Cleanup Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(clear_error_frees_memory) {
    struct bj_error* err = 0;
    bj_set_error(&err, BJ_ERROR, "test");
    REQUIRE_VALUE(err);

    bj_clear_error(&err);
    REQUIRE_NULL(err);
}

TEST_CASE(clear_error_on_null_is_safe) {
    bj_clear_error(0);

    struct bj_error* err = 0;
    bj_clear_error(&err);

    REQUIRE(1); // No crash
}

TEST_CASE(double_clear_is_safe) {
    struct bj_error* err = 0;
    bj_set_error(&err, BJ_ERROR, "test");

    bj_clear_error(&err);
    bj_clear_error(&err); // Should be safe

    REQUIRE_NULL(err);
}

////////////////////////////////////////////////////////////////////////////////
// Copy Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(copy_error_duplicates_all_fields) {
    struct bj_error* original = 0;
    bj_set_error(&original, BJ_ERROR_INVALID_FORMAT, "bad format");

    struct bj_error* copy = bj_copy_error(original);

    REQUIRE_VALUE(copy);
    REQUIRE_EQ(bj_error_code(copy), bj_error_code(original));
    REQUIRE(strcmp(bj_error_message(copy), bj_error_message(original)) == 0);

    // They should be independent
    REQUIRE_NEQ(copy, original);

    bj_clear_error(&original);
    bj_clear_error(&copy);
}

TEST_CASE(copy_null_error_returns_null) {
    struct bj_error* copy = bj_copy_error(0);
    REQUIRE_NULL(copy);
}

////////////////////////////////////////////////////////////////////////////////
// Prefix Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(prefix_error_prepends_message) {
    struct bj_error* err = 0;
    bj_set_error(&err, BJ_ERROR, "original message");

    bj_prefix_error(&err, "Context: ");

    REQUIRE(strstr(bj_error_message(err), "Context:") != 0);
    REQUIRE(strstr(bj_error_message(err), "original message") != 0);

    bj_clear_error(&err);
}

TEST_CASE(prefix_error_fmt_formats_prefix) {
    struct bj_error* err = 0;
    bj_set_error(&err, BJ_ERROR, "inner");

    bj_prefix_error_fmt(&err, "Item %d: ", 42);

    REQUIRE(strstr(bj_error_message(err), "42") != 0);
    REQUIRE(strstr(bj_error_message(err), "inner") != 0);

    bj_clear_error(&err);
}

TEST_CASE(prefix_error_on_null_is_safe) {
    struct bj_error* err = 0;
    bj_prefix_error(&err, "prefix");
    bj_prefix_error_fmt(&err, "prefix %d", 1);
    REQUIRE_NULL(err); // Should remain null
}

////////////////////////////////////////////////////////////////////////////////
// Error Code Structure Tests
////////////////////////////////////////////////////////////////////////////////

TEST_CASE(error_code_kind_macro_works) {
    // Test that the kind extraction macro works correctly
    REQUIRE_EQ(bj_error_code_kind(BJ_ERROR_NONE), 0x00);
    REQUIRE_EQ(bj_error_code_kind(BJ_ERROR), 0x01);
    REQUIRE_EQ(bj_error_code_kind(BJ_ERROR_SYSTEM), 0x02);
    REQUIRE_EQ(bj_error_code_kind(BJ_ERROR_FILE_NOT_FOUND), 0x02); // Same kind as SYSTEM
    REQUIRE_EQ(bj_error_code_kind(BJ_ERROR_IO), 0x03);
    REQUIRE_EQ(bj_error_code_kind(BJ_ERROR_CANNOT_READ), 0x03); // Same kind as IO
    REQUIRE_EQ(bj_error_code_kind(BJ_ERROR_INVALID_DATA), 0x04);
}

TEST_CASE(error_code_is_user_macro_works) {
    // Banjo errors have MSB = 0x00
    REQUIRE_EQ(bj_error_code_is_user(BJ_ERROR), 0);
    REQUIRE_EQ(bj_error_code_is_user(BJ_ERROR_FILE_NOT_FOUND), 0);

    // User-defined errors would have non-zero MSB
    REQUIRE_NEQ(bj_error_code_is_user(0x01000000), 0);
    REQUIRE_NEQ(bj_error_code_is_user(0xFF000001), 0);
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    // Creation
    RUN_TEST(set_error_stores_code_and_message);
    RUN_TEST(set_error_fmt_formats_correctly);
    RUN_TEST(set_error_with_null_destination_is_safe);
    RUN_TEST(set_error_first_error_wins);

    // Propagation
    RUN_TEST(propagate_error_transfers_ownership);
    RUN_TEST(propagate_prefixed_error_prepends_message);
    RUN_TEST(propagate_to_null_clears_source);

    // Inspection
    RUN_TEST(error_matches_returns_true_for_exact_code);
    RUN_TEST(error_matches_kind_checks_category);
    RUN_TEST(error_code_on_null_returns_none);
    RUN_TEST(error_message_on_null_returns_null);
    RUN_TEST(error_matches_on_null_returns_false);

    // Cleanup
    RUN_TEST(clear_error_frees_memory);
    RUN_TEST(clear_error_on_null_is_safe);
    RUN_TEST(double_clear_is_safe);

    // Copy
    RUN_TEST(copy_error_duplicates_all_fields);
    RUN_TEST(copy_null_error_returns_null);

    // Prefix
    RUN_TEST(prefix_error_prepends_message);
    RUN_TEST(prefix_error_fmt_formats_prefix);
    RUN_TEST(prefix_error_on_null_is_safe);

    // Error code structure
    RUN_TEST(error_code_kind_macro_works);
    RUN_TEST(error_code_is_user_macro_works);

    END_TESTS();
}
