#include "test.h"

#include "rbuffer_t.h"

#include <string.h>

typedef double payload;

#define BUCKET_SIZE 16

TEST_CASE(initialize_with_payload_gives_empty_rbuffer) {
    bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload, BUCKET_SIZE);
    REQUIRE_VALUE(p_rbuffer);
    REQUIRE_EQ(p_rbuffer->item_payload, sizeof(payload));
    REQUIRE_EQ(p_rbuffer->bucket_size, BUCKET_SIZE);
    REQUIRE_EQ(p_rbuffer->n_buckets, 0);
    REQUIRE_EQ(p_rbuffer->buckets, 0);
    bj_rbuffer_del(p_rbuffer);
}

TEST_CASE(reset_zero_entire_object) {
    bj_rbuffer rbuffer = {
        .item_payload = sizeof(payload),
        .bucket_size  = BUCKET_SIZE,
    };
    rbuffer.n_buckets = 0;
    bj_rbuffer_reset(&rbuffer);
    REQUIRE_NIL(bj_rbuffer, &rbuffer);
}

TEST_CASE(initialize_with_0_payload_returns_0) {
    bj_rbuffer* p_rbuffer = bj_rbuffer_new(0, BUCKET_SIZE);
    REQUIRE_NULL(p_rbuffer);
}

TEST_CASE(initialize_with_0_bucket_size_returns_0) {
    bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload, 0);
    REQUIRE_NULL(p_rbuffer);
}

TEST_CASE(capacity_null_returns_0) {
    REQUIRE_EQ(bj_rbuffer_capacity(0), 0);
}

TEST_CASE(capacity_empty_returns_0) {
    bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload, BUCKET_SIZE);
    REQUIRE_EQ(bj_rbuffer_capacity(p_rbuffer), 0);
    bj_rbuffer_del(p_rbuffer);
}

TEST_CASE(reserve_null_returns_0) {
    REQUIRE_EQ(bj_rbuffer_reserve(0, 42), 0);
}

TEST_CASE(reserve_larger_growths_capacity) {
    bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload, BUCKET_SIZE);
    for(size_t c = 1 ; c < (BUCKET_SIZE + 1) ; ++c) {
        const size_t req_cap = c * (BUCKET_SIZE + 1); 
        const size_t act_cap = bj_rbuffer_reserve(p_rbuffer, req_cap);

        REQUIRE(act_cap >= req_cap); // At least required capacity

        REQUIRE_EQ(act_cap % BUCKET_SIZE, 0); // cap is multiple of bucket size

        const size_t got_cap = bj_rbuffer_capacity(p_rbuffer);
        REQUIRE_EQ(got_cap, act_cap); // Consistent returned capacity
    }
    bj_rbuffer_del(p_rbuffer);
}

TEST_CASE(reserve_smaller_does_not_change_buffer) {
    bj_rbuffer* p_rbuffer = bj_rbuffer_new_t(payload, BUCKET_SIZE);

    const size_t ini_cap = bj_rbuffer_reserve(p_rbuffer, 10);
    REQUIRE(ini_cap >= 10);

    const size_t expected_n_buckets = p_rbuffer->n_buckets;
    const struct bj_rbucket_t* expected_p_buckets = p_rbuffer->buckets;

    const size_t new_cap = bj_rbuffer_reserve(p_rbuffer, 3);

    REQUIRE_EQ(new_cap, ini_cap);
    REQUIRE_EQ(p_rbuffer->n_buckets, expected_n_buckets);
    REQUIRE_EQ(p_rbuffer->buckets, expected_p_buckets);

    bj_rbuffer_del(p_rbuffer);
}


int main(int argc, char* argv[]) {
    BEGIN_TESTS(argc, argv);

    RUN_TEST(initialize_with_payload_gives_empty_rbuffer);
    RUN_TEST(initialize_with_0_payload_returns_0);
    RUN_TEST(initialize_with_0_bucket_size_returns_0);
    RUN_TEST(reset_zero_entire_object);
    RUN_TEST(capacity_null_returns_0);
    RUN_TEST(capacity_empty_returns_0);
    RUN_TEST(reserve_null_returns_0);
    RUN_TEST(reserve_larger_growths_capacity);
    RUN_TEST(reserve_smaller_does_not_change_buffer);

    END_TESTS();
}

