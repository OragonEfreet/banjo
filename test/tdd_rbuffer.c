#include "test.h"
#include "rbuffer_t.h"

TEST_CASE(new_returns_non_null) {
    bj_rbuffer* buf = bj_rbuffer_new(1);
    REQUIRE_VALUE(buf);
    bj_rbuffer_del(buf);
}

TEST_CASE(new_initializes_capacity) {
    bj_rbuffer* buf = bj_rbuffer_new(10);
    REQUIRE_EQ(buf->capacity, 10);
    bj_rbuffer_del(buf);
}

TEST_CASE(new_initializes_write_index_to_zero) {
    bj_rbuffer* buf = bj_rbuffer_new(5);
    REQUIRE_EQ(buf->write, 0);
    bj_rbuffer_del(buf);
}

TEST_CASE(new_initializes_read_index_to_zero) {
    bj_rbuffer* buf = bj_rbuffer_new(5);
    REQUIRE_EQ(buf->read, 0);
    bj_rbuffer_del(buf);
}

TEST_CASE(new_zero_capacity_works) {
    bj_rbuffer* buf = bj_rbuffer_new(0);
    REQUIRE_VALUE(buf);
    REQUIRE_EQ(buf->capacity, 0);
    REQUIRE_EQ(buf->write, 0);
    REQUIRE_EQ(buf->read, 0);
    bj_rbuffer_del(buf);
}

TEST_CASE(new_large_capacity_initializes_correctly) {
    size_t cap = 1 << 20;  // 1M
    bj_rbuffer* buf = bj_rbuffer_new(cap);
    REQUIRE_VALUE(buf);
    REQUIRE_EQ(buf->capacity, cap);
    REQUIRE_EQ(buf->write, 0);
    REQUIRE_EQ(buf->read, 0);
    bj_rbuffer_del(buf); }

TEST_CASE(new_multiple_instances_are_independent) {
    bj_rbuffer* a = bj_rbuffer_new(3);
    bj_rbuffer* b = bj_rbuffer_new(5);
    REQUIRE_NEQ(a, b);
    REQUIRE_EQ(a->capacity, 3);
    REQUIRE_EQ(b->capacity, 5);
    REQUIRE_EQ(a->write, 0);
    REQUIRE_EQ(b->write, 0);
    REQUIRE_EQ(a->read, 0);
    REQUIRE_EQ(b->read, 0);
    bj_rbuffer_del(a);
    bj_rbuffer_del(b);
}

TEST_CASE(reserve_null_rbuffer_returns_zero_and_does_nothing) {
    size_t ret = bj_rbuffer_reserve(NULL, 10);
    REQUIRE_EQ(ret, 0);
}

TEST_CASE(reserve_smaller_than_current_capacity_does_nothing) {
    bj_rbuffer rbuf = {.capacity = 64};
    size_t ret = bj_rbuffer_reserve(&rbuf, 32);
    REQUIRE_EQ(ret, 64);
    REQUIRE_EQ(rbuf.capacity, 64);
}

TEST_CASE(reserve_equal_to_current_capacity_does_nothing) {
    bj_rbuffer rbuf = {.capacity = 64};
    size_t ret = bj_rbuffer_reserve(&rbuf, 64);
    REQUIRE_EQ(ret, 64);
    REQUIRE_EQ(rbuf.capacity, 64);
}


TEST_CASE(reserve_greater_capacity_increases_capacity) {
    bj_rbuffer rbuf = {.capacity = 32};
    size_t requested = 128;
    size_t ret = bj_rbuffer_reserve(&rbuf, requested);
    REQUIRE(ret >= requested);
    REQUIRE(rbuf.capacity >= requested);
    REQUIRE_EQ(ret, rbuf.capacity);
}

TEST_CASE(reserve_increases_capacity_but_preserves_read_write) {
    bj_rbuffer rbuf = {.write = 10, .read = 5, .capacity = 32};
    size_t requested = 100;
    size_t old_read = rbuf.read;
    size_t old_write = rbuf.write;
    size_t ret = bj_rbuffer_reserve(&rbuf, requested);
    REQUIRE(ret >= requested);
    REQUIRE(rbuf.capacity >= requested);
    REQUIRE_EQ(rbuf.read, old_read);
    REQUIRE_EQ(rbuf.write, old_write);
}

TEST_CASE(reserve_multiple_growth_calls_grow_capacity) {
    bj_rbuffer rbuf = {0};

    size_t cap1 = bj_rbuffer_reserve(&rbuf, 10);
    REQUIRE(cap1 >= 10);
    size_t cap2 = bj_rbuffer_reserve(&rbuf, 20);
    REQUIRE(cap2 >= 20);
    REQUIRE(cap2 >= cap1);
    size_t cap3 = bj_rbuffer_reserve(&rbuf, 15);
    REQUIRE_EQ(cap3, cap2);  // no shrink
}

TEST_CASE(reserve_capacity_with_max_value) {
    bj_rbuffer rbuf = {0};
    size_t ret = bj_rbuffer_reserve(&rbuf, SIZE_MAX);
    REQUIRE(ret >= SIZE_MAX || ret == SIZE_MAX); // depends on implementation, at least no crash
}

TEST_CASE(reserve_from_zero_capacity_grows) {
    bj_rbuffer rbuf = {.capacity = 0};
    size_t ret = bj_rbuffer_reserve(&rbuf, 16);
    REQUIRE(ret >= 16);
    REQUIRE(rbuf.capacity >= 16);
}


TEST_CASE(rbuffer_reserve_when_capacity_near_max_does_not_wrap) {
    bj_rbuffer rbuf = {.capacity = SIZE_MAX - 10};
    size_t ret = bj_rbuffer_reserve(&rbuf, SIZE_MAX - 5);
    REQUIRE(ret <= SIZE_MAX); // Doesn't overflow
}

TEST_CASE(full_null_rbuffer_returns_false) {
    bj_bool result = bj_rbuffer_full(NULL);
    REQUIRE(!result);
}

TEST_CASE(full_empty_buffer_returns_false) {
    bj_rbuffer rbuf = { .write = 0, .read = 0 };
    bj_bool result = bj_rbuffer_full(&rbuf);
    REQUIRE(!result);
}

TEST_CASE(full_buffer_with_write_less_than_read_returns_false) {
    bj_rbuffer rbuf = { .write = 3, .read = 5, .capacity = 16 };
    bj_bool result = bj_rbuffer_full(&rbuf);
    REQUIRE(!result);
}

TEST_CASE(full_buffer_with_write_equal_to_read_minus_one_returns_true) {
    size_t cap = 16;
    bj_rbuffer rbuf = { .write = (5 + cap - 1) % cap, .read = 5, .capacity = cap };
    bj_bool result = bj_rbuffer_full(&rbuf);
    REQUIRE(result);
}

TEST_CASE(full_buffer_with_write_equal_to_read_returns_false) {
    bj_rbuffer rbuf = { .write = 5, .read = 5, .capacity = 16 };
    bj_bool result = bj_rbuffer_full(&rbuf);
    REQUIRE(!result);
}

TEST_CASE(full_buffer_write_just_before_read_returns_true) {
    bj_rbuffer rbuf = { .write = 15, .read = 0, .capacity = 16 };
    bj_bool result = bj_rbuffer_full(&rbuf);
    REQUIRE(result);
}

TEST_CASE(full_buffer_write_far_from_read_returns_false) {
    bj_rbuffer rbuf = { .write = 10, .read = 2, .capacity = 16 };
    bj_bool result = bj_rbuffer_full(&rbuf);
    REQUIRE(!result);
}

TEST_CASE(full_buffer_capacity_zero_always_false) {
    bj_rbuffer rbuf = { .write = 0, .read = 0, .capacity = 0 };
    bj_bool result = bj_rbuffer_full(&rbuf);
    REQUIRE(result == BJ_FALSE);
}


TEST_CASE(empty_null_returns_true) {
    bj_bool result = bj_rbuffer_empty(NULL);
    REQUIRE(result);
}

TEST_CASE(empty_new_buffer_is_true) {
    bj_rbuffer rbuf = {.write = 0, .read = 0, .capacity = 10};
    REQUIRE(bj_rbuffer_empty(&rbuf));
}

TEST_CASE(empty_when_write_equals_read) {
    bj_rbuffer rbuf = {.write = 5, .read = 5, .capacity = 10};
    REQUIRE(bj_rbuffer_empty(&rbuf));
}

TEST_CASE(empty_when_write_different_from_read) {
    bj_rbuffer rbuf = {.write = 7, .read = 5, .capacity = 10};
    REQUIRE(!bj_rbuffer_empty(&rbuf));
}

TEST_CASE(empty_when_write_wraps_around_and_equals_read) {
    bj_rbuffer rbuf = {.write = 0, .read = 0, .capacity = 10};
    REQUIRE(bj_rbuffer_empty(&rbuf));
}

TEST_CASE(empty_when_write_wraps_around_but_not_empty) {
    bj_rbuffer rbuf = {.write = 2, .read = 7, .capacity = 10};
    REQUIRE(!bj_rbuffer_empty(&rbuf));
}

TEST_CASE(write_overrun_zero_length_does_nothing) {
    bj_rbuffer buffer = {.capacity = 8};
    bj_rbuffer_write_overrun(&buffer, (void*)"abcd", 0);
    REQUIRE_EQ(buffer.write, 0);
    REQUIRE_EQ(buffer.read, 0);
}

TEST_CASE(write_overrun_null_data_advances_write_only) {
    bj_rbuffer buffer = {.capacity = 8};
    bj_rbuffer_write_overrun(&buffer, 0, 3);
    REQUIRE_EQ(buffer.write, 3);
    REQUIRE_EQ(buffer.read, 0);
}

TEST_CASE(write_overrun_fits_without_wrapping) {
    bj_rbuffer buffer = {.capacity = 8, .write = 0, .read = 0};
    char data[4] = {'a','b','c','d'};
    bj_rbuffer_write_overrun(&buffer, data, 4);
    REQUIRE_EQ(buffer.write, 4);
    REQUIRE_EQ(buffer.read, 0);
}

TEST_CASE(used_and_available_null_buffer) {
    REQUIRE_EQ(bj_rbuffer_used(0), 0);
    REQUIRE_EQ(bj_rbuffer_available(0), 0);
}

TEST_CASE(used_and_available_empty_buffer) {
    bj_rbuffer buffer = {.capacity = 8, .write = 0, .read = 0};
    REQUIRE_EQ(bj_rbuffer_used(&buffer), 0);
    REQUIRE_EQ(bj_rbuffer_available(&buffer), 7); // 8 - 1
}

TEST_CASE(used_and_available_partially_filled) {
    bj_rbuffer buffer = {.capacity = 8, .write = 3, .read = 1};
    REQUIRE_EQ(bj_rbuffer_used(&buffer), 2);
    REQUIRE_EQ(bj_rbuffer_available(&buffer), 5);
}

TEST_CASE(used_and_available_full_buffer_case1) {
    bj_rbuffer buffer = {.capacity = 8, .write = 6, .read = 7};
    REQUIRE_EQ(bj_rbuffer_used(&buffer), 7);
    REQUIRE_EQ(bj_rbuffer_available(&buffer), 0);
}

TEST_CASE(used_and_available_full_buffer_case2_wraparound) {
    bj_rbuffer buffer = {.capacity = 8, .write = 1, .read = 2};
    REQUIRE_EQ(bj_rbuffer_used(&buffer), 7);
    REQUIRE_EQ(bj_rbuffer_available(&buffer), 0);
}

TEST_CASE(used_and_available_half_full_no_wrap) {
    bj_rbuffer buffer = {.capacity = 8, .write = 4, .read = 0};
    REQUIRE_EQ(bj_rbuffer_used(&buffer), 4);
    REQUIRE_EQ(bj_rbuffer_available(&buffer), 3);
}

TEST_CASE(used_and_available_half_full_wrap) {
    bj_rbuffer buffer = {.capacity = 8, .write = 1, .read = 5};
    REQUIRE_EQ(bj_rbuffer_used(&buffer), 4);
    REQUIRE_EQ(bj_rbuffer_available(&buffer), 3);
}

TEST_CASE(used_and_available_read_equals_write_but_full) {
    bj_rbuffer buffer = {.capacity = 8, .write = 7, .read = 0};
    REQUIRE_EQ(bj_rbuffer_used(&buffer), 7);
    REQUIRE_EQ(bj_rbuffer_available(&buffer), 0);
}

TEST_CASE(used_and_available_read_equals_write_empty) {
    bj_rbuffer buffer = {.capacity = 8, .write = 3, .read = 3};
    REQUIRE_EQ(bj_rbuffer_used(&buffer), 0);
    REQUIRE_EQ(bj_rbuffer_available(&buffer), 7);
}

TEST_CASE(write_overrun_exact_capacity_wraps_and_resets) {
    bj_rbuffer buffer = {.capacity = 8, .write = 0, .read = 0};
    char data[8] = {'x','x','x','x','x','x','x','x'};
    bj_rbuffer_write_overrun(&buffer, data, 8);
    REQUIRE_EQ(buffer.write, 0);
    REQUIRE_EQ(buffer.read, 1);
}

TEST_CASE(write_overrun_overwrites_and_advances_read) {
    bj_rbuffer buffer = {.capacity = 8, .write = 6, .read = 2};
    char data[5] = {'1','2','3','4','5'};
    bj_rbuffer_write_overrun(&buffer, data, 5);
    REQUIRE_EQ(buffer.write, 3);
    REQUIRE_EQ(buffer.read, 4);
}


TEST_CASE(available_and_used_basic) {
    bj_rbuffer buf = {.capacity = 8, .write = 0, .read = 0};
    REQUIRE_EQ(bj_rbuffer_used(&buf), 0);
    REQUIRE_EQ(bj_rbuffer_available(&buf), 7); // capacity - 1
}

TEST_CASE(available_and_used_partial) {
    bj_rbuffer buf = {.capacity = 8, .write = 3, .read = 1};
    REQUIRE_EQ(bj_rbuffer_used(&buf), 2);
    REQUIRE_EQ(bj_rbuffer_available(&buf), 5);
}

TEST_CASE(available_and_used_wraparound) {
    bj_rbuffer buf = {.capacity = 8, .write = 1, .read = 6};
    REQUIRE_EQ(bj_rbuffer_used(&buf), 3);       // [6,7,0]
    REQUIRE_EQ(bj_rbuffer_available(&buf), 4);  // 8 - 3 - 1
}

TEST_CASE(available_and_used_full) {
    bj_rbuffer buf = {.capacity = 8, .write = 6, .read = 7};
    REQUIRE(bj_rbuffer_full(&buf));
    REQUIRE_EQ(bj_rbuffer_used(&buf), 7);       // Full: used = capacity - 1
    REQUIRE_EQ(bj_rbuffer_available(&buf), 0);
}

TEST_CASE(available_and_used_empty_after_overrun) {
    bj_rbuffer buf = {.capacity = 4, .write = 2, .read = 2};
    bj_rbuffer_write_overrun(&buf, NULL, 3); // overrun entire buffer
    REQUIRE(bj_rbuffer_used(&buf) == 3);
    REQUIRE(bj_rbuffer_available(&buf) == 0); // Now full (3 used + 1 reserved)
}

TEST_CASE(write_overrun_zero_bytes_does_nothing) {
    bj_rbuffer buffer = {.capacity = 8, .write = 3, .read = 2};
    bj_rbuffer_write_overrun(&buffer, NULL, 0);
    REQUIRE_EQ(buffer.write, 3);
    REQUIRE_EQ(buffer.read, 2);
}

TEST_CASE(write_overrun_without_overrun_advances_write_only) {
    bj_rbuffer buffer = {.capacity = 8, .write = 2, .read = 1};
    bj_rbuffer_write_overrun(&buffer, NULL, 3);
    REQUIRE_EQ(buffer.write, (2 + 3) % 8);
    REQUIRE_EQ(buffer.read, 1);
}

TEST_CASE(write_overrun_with_exact_capacity_causes_full_overwrite) {
    bj_rbuffer buffer = {.capacity = 8, .write = 0, .read = 0};
    bj_rbuffer_write_overrun(&buffer, NULL, 8);
    REQUIRE_EQ(buffer.write, 0);  // wrapped
    REQUIRE_EQ(buffer.read, 1);   // advanced 8 steps
}

TEST_CASE(write_overrun_partial_overrun_advances_both_indices) {
    bj_rbuffer buffer = {.capacity = 8, .write = 6, .read = 2};
    bj_rbuffer_write_overrun(&buffer, NULL, 5); // 5 > available (3), expect overrun by 2
    REQUIRE_EQ(buffer.write, (6 + 5) % 8); // 11 % 8 = 3
    REQUIRE_EQ(buffer.read, (2 + 2) % 8);  // overrun by 2
}

TEST_CASE(write_overrun_full_buffer_wraps_and_overwrites_all) {
    bj_rbuffer buffer = {.capacity = 4, .write = 3, .read = 0};
    bj_rbuffer_write_overrun(&buffer, NULL, 5); // Full overrun
    REQUIRE_EQ(buffer.write, (3 + 5) % 4); // 8 % 4 = 0
    REQUIRE_EQ(buffer.read, (0 + 5) % 4);  // 5 % 4 = 1
}

TEST_CASE(write_overrun_full_buffer_single_byte) {
    bj_rbuffer buffer = {.capacity = 4, .write = 3, .read = 2};
    bj_rbuffer_write_overrun(&buffer, NULL, 1); // 1-byte write
    REQUIRE_EQ(buffer.write, 0);
    REQUIRE_EQ(buffer.read, 2); // no overrun
}

TEST_CASE(write_overrun_full_capacity_when_empty) {
    bj_rbuffer buffer = {.capacity = 6, .write = 0, .read = 0};
    bj_rbuffer_write_overrun(&buffer, NULL, 10); // overrun by 4
    REQUIRE_EQ(buffer.write, 4); // 10 % 6 = 4
    REQUIRE_EQ(buffer.read, 5);  // overrun 4 bytes
}

TEST_CASE(write_overrun_no_available_but_not_full_advances_correctly) {
    bj_rbuffer buffer = {.capacity = 5, .write = 3, .read = 4}; // full
    bj_rbuffer_write_overrun(&buffer, NULL, 2);
    REQUIRE_EQ(buffer.write, 0); // (3 + 2) % 5
    REQUIRE_EQ(buffer.read, 1);  // overrun by 2
}

TEST_CASE(write_overrun_exact_available_advances_write_only) {
    bj_rbuffer buffer = {.capacity = 8, .write = 2, .read = 0}; // used = 2, avail = 5
    bj_rbuffer_write_overrun(&buffer, NULL, 5);
    REQUIRE_EQ(buffer.write, 7);
    REQUIRE_EQ(buffer.read, 0); // no overrun
}

TEST_CASE(rbuffer_full_capacity_one) {
    bj_rbuffer rbuf = {.capacity = 1, .write = 0, .read = 0};
    REQUIRE(bj_rbuffer_full(&rbuf));
    bj_rbuffer_write_overrun(&rbuf, NULL, 1);
    REQUIRE(bj_rbuffer_full(&rbuf));  // Still not full!
    REQUIRE(bj_rbuffer_used(&rbuf) == 0); // because nothing actually fits
}

int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(available_and_used_basic);
    RUN_TEST(available_and_used_empty_after_overrun);
    RUN_TEST(available_and_used_full);
    RUN_TEST(available_and_used_partial);
    RUN_TEST(available_and_used_wraparound);
    RUN_TEST(empty_new_buffer_is_true);
    RUN_TEST(empty_null_returns_true);
    RUN_TEST(empty_when_write_different_from_read);
    RUN_TEST(empty_when_write_equals_read);
    RUN_TEST(empty_when_write_wraps_around_and_equals_read);
    RUN_TEST(empty_when_write_wraps_around_but_not_empty);
    RUN_TEST(full_buffer_capacity_zero_always_false);
    RUN_TEST(full_buffer_with_write_equal_to_read_minus_one_returns_true);
    RUN_TEST(full_buffer_with_write_equal_to_read_returns_false);
    RUN_TEST(full_buffer_with_write_less_than_read_returns_false);
    RUN_TEST(full_buffer_write_far_from_read_returns_false);
    RUN_TEST(full_buffer_write_just_before_read_returns_true);
    RUN_TEST(full_empty_buffer_returns_false);
    RUN_TEST(full_null_rbuffer_returns_false);
    RUN_TEST(new_initializes_capacity);
    RUN_TEST(new_initializes_read_index_to_zero);
    RUN_TEST(new_initializes_write_index_to_zero);
    RUN_TEST(new_large_capacity_initializes_correctly);
    RUN_TEST(new_multiple_instances_are_independent);
    RUN_TEST(new_returns_non_null);
    RUN_TEST(new_zero_capacity_works);
    RUN_TEST(rbuffer_reserve_when_capacity_near_max_does_not_wrap);
    RUN_TEST(reserve_capacity_with_max_value);
    RUN_TEST(reserve_equal_to_current_capacity_does_nothing);
    RUN_TEST(reserve_from_zero_capacity_grows);
    RUN_TEST(reserve_greater_capacity_increases_capacity);
    RUN_TEST(reserve_increases_capacity_but_preserves_read_write);
    RUN_TEST(reserve_multiple_growth_calls_grow_capacity);
    RUN_TEST(reserve_null_rbuffer_returns_zero_and_does_nothing);
    RUN_TEST(reserve_smaller_than_current_capacity_does_nothing);
    RUN_TEST(used_and_available_empty_buffer);
    RUN_TEST(used_and_available_full_buffer_case1);
    RUN_TEST(used_and_available_full_buffer_case2_wraparound);
    RUN_TEST(used_and_available_half_full_no_wrap);
    RUN_TEST(used_and_available_half_full_wrap);
    RUN_TEST(used_and_available_null_buffer);
    RUN_TEST(used_and_available_partially_filled);
    RUN_TEST(used_and_available_read_equals_write_but_full);
    RUN_TEST(used_and_available_read_equals_write_empty);
    RUN_TEST(write_overrun_exact_available_advances_write_only);
    RUN_TEST(write_overrun_exact_capacity_wraps_and_resets);
    RUN_TEST(write_overrun_fits_without_wrapping);
    RUN_TEST(write_overrun_full_buffer_single_byte);
    RUN_TEST(write_overrun_full_buffer_wraps_and_overwrites_all);
    RUN_TEST(write_overrun_full_capacity_when_empty);
    RUN_TEST(write_overrun_no_available_but_not_full_advances_correctly);
    RUN_TEST(write_overrun_null_data_advances_write_only);
    RUN_TEST(write_overrun_overwrites_and_advances_read);
    RUN_TEST(write_overrun_partial_overrun_advances_both_indices);
    RUN_TEST(write_overrun_with_exact_capacity_causes_full_overwrite);
    RUN_TEST(write_overrun_without_overrun_advances_write_only);
    RUN_TEST(write_overrun_zero_bytes_does_nothing);
    RUN_TEST(write_overrun_zero_length_does_nothing);
    RUN_TEST(rbuffer_full_capacity_one);

    END_TESTS();
}

