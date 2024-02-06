#pragma once

// TODO: + [Opt] Failing tests don't terminate program
// TODO: + Tag system
// TODO: + ShouldFail system

#include <stdio.h>

// Symbol overrides
#define SM_NS(SM) test_ ## SM
#define SM_TST_FN(NAME) SM_NS(fn_ ## NAME)
#define TST_FLAGS(NAME) SM_NS(flags_ ## NAME)
#define SM_CTX() SM_NS(context)
#define SM_FLAGS_PARAM() SM_NS(flags)

// Behaviour overrides
#ifndef PRINT
#   define PRINT(...) printf(__VA_ARGS__)
#endif

typedef enum {
    EXECUTED = 1,
    FAIL = 2,
} StatusFlag;

typedef struct {
    char* prog_name;
    unsigned char n_run;
    unsigned char n_fail;
    void* user_data;
    int stop_at_err;
} Context;


void initialize_context(Context* context, int argc, char* argv[]) {
    if(argc >= 1) {
        context->prog_name = argv[0];
    } else {
        context->prog_name = "UnitTest";
    }
}

int record_test_result(Context* context, int status_flag) {
    ++context->n_run;
    if ((status_flag & FAIL) > 0) {
        ++context->n_fail;
        return 1 & context->stop_at_err;
    }
    return 0;
}

int terminate_context(Context* SM_CTX()) {
    const char* fmt = SM_CTX()->stop_at_err ? "Ran up to %d test cases. %d failed\n" : "Ran %d test cases. %d failed.\n";
    PRINT(fmt, SM_CTX()->n_run, SM_CTX()->n_fail);
    return SM_CTX()->n_fail;
}

// INTERNAL
#define DEFINE_TST_FN(NAME) void SM_TST_FN(NAME)(Context* SM_CTX(), StatusFlag* SM_FLAGS_PARAM())
#define DECL_TST_FLAGS(NAME) static StatusFlag TST_FLAGS(NAME) = 0

#define TRACE(NAME, COND) PRINT("%s:%d: FAILED (%s): %s \n", SM_CTX()->prog_name, __LINE__, #NAME, #COND);
#define ON_FAIL(NAME, COND) TRACE(NAME, COND); 

#define CHECK_TEST(NAME) if (record_test_result(&SM_CTX(), TST_FLAGS(NAME))) END_TESTS();

// PUBLIC
#define BEGIN_TESTS(ARGC, ARGV) Context SM_CTX() = {}; initialize_context(&SM_CTX(), ARGC, ARGV)
#define END_TESTS() return terminate_context(&SM_CTX())

#define TEST_CASE(NAME) DECL_TST_FLAGS(NAME); DEFINE_TST_FN(NAME)

#define RUN_TEST(NAME) SM_TST_FN(NAME)(&SM_CTX(), &TST_FLAGS(NAME));CHECK_TEST(NAME);

#define REQUIRE(COND) if(!(COND)) {ON_FAIL(REQUIRE, COND);*SM_FLAGS_PARAM() |= FAIL;return;}
#define CHECK(COND) if(!(COND)) {TRACE(CHECK, COND);}

