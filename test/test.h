#pragma once

// TODO: + No need to name tests anymore
// TODO: + Tag system
// TODO: + ShouldFail system
// TODO: + Use __COUNTER__ to get rid of RUN_TEST(x)?


// Symbol overrides
#define SM_NS(SM) test_ ## SM
#define SM_TST_FN(NAME) SM_NS(fn_ ## NAME) 
#define TST_FLAGS(NAME) SM_NS(flags_ ## NAME) 
#define SM_CTX() SM_NS(context) 
#define SM_FLAGS_PARAM() SM_NS(flags) 

// Behaviour overrides
// PRINT. Same signature as printf().
// Change it to anithing you want if needed
#ifndef PRINT
#   include <stdio.h>
#   define PRINT(...) printf(__VA_ARGS__)
#endif

typedef enum {
    EXECUTED = 1,
    FAIL     = 2,
} StatusFlag;

typedef struct {
    char*         prog_name;   // Name of the curernt test progrm
    unsigned char n_run;       // Number of test cases already finished
    unsigned char n_fail;      // Number of failed test cases
    int           stop_at_err; // If 0, program continues on error
    void*         user_data;   // User data
} Context;


// Parses ARGC/ARGV and builds the COntext object
void initialize_context(Context* context, int argc, char* argv[]) {
    if(argc >= 1) {
        context->prog_name = argv[0];
    } else {
        context->prog_name = "UnitTest";
    }
}

// Called after each test ends
// Returns 1 means "terminate program"
int record_test_result(Context* context, int status_flag) {
    ++context->n_run;
    if ((status_flag & FAIL) > 0) {
        ++context->n_fail;
        return 1 & context->stop_at_err;
    }
    return 0;
}

// Terminate the test program, outputs a report
// Return 0 means all test passed
int terminate_context(Context* SM_CTX()) {
    const char* fmt = SM_CTX()->stop_at_err ? "Ran up to %d test cases. %d failed\n" : "Ran %d test cases. %d failed.\n";
    PRINT(fmt, SM_CTX()->n_run, SM_CTX()->n_fail);
    return SM_CTX()->n_fail;
}

////////////////////////////////////////////////////////////////////////////////
// INTERNAL MACROS
#define DEFINE_TST_FN(NAME) void SM_TST_FN(NAME)(Context* SM_CTX(), StatusFlag* SM_FLAGS_PARAM())
#define DECL_TST_FLAGS(NAME) static StatusFlag TST_FLAGS(NAME) = 0
#define TRACE(NAME, STR, ...) PRINT("%s:%d: FAILED (%s): " STR "\n", SM_CTX()->prog_name, __LINE__, #NAME, __VA_ARGS__);
#define CHECK_TEST(NAME) if (record_test_result(&SM_CTX(), TST_FLAGS(NAME))) END_TESTS();

#define NOPE ;
#define STOP *SM_FLAGS_PARAM() |= FAIL;return;

#define DO_CHECK(CMD, COND, IF_FAIL) if(!(COND)) { TRACE(CMD, "%s", #COND); IF_FAIL; }
#define DO_CHECK_EQ(CMD, RES, EXPECTED, IF_FAIL)  if(!(RES == EXPECTED)) { TRACE(CMD, "%s: expected %s", #RES, #EXPECTED); IF_FAIL; } 
#define DO_CHECK_NEQ(CMD, RES, EXPECTED, IF_FAIL)  if((RES == EXPECTED)) { TRACE(CMD, "%s: expected NOT %s", #RES, #EXPECTED); IF_FAIL; } 
#define DO_CHECK_NULL(CMD, RES, IF_FAIL)  if(!(RES == 0)) { TRACE(CMD, "%s", #RES); IF_FAIL; } 
#define DO_CHECK_VALUE(CMD, RES, IF_FAIL)  if((RES == 0)) { TRACE(CMD, "%s", #RES); IF_FAIL; } 

////////////////////////////////////////////////////////////////////////////////
// PUBLIC
#define BEGIN_TESTS(ARGC, ARGV) Context SM_CTX() = {}; initialize_context(&SM_CTX(), ARGC, ARGV)
#define END_TESTS() return terminate_context(&SM_CTX())

#define TEST_CASE(NAME) DECL_TST_FLAGS(NAME); DEFINE_TST_FN(NAME)

#define RUN_TEST(NAME) SM_TST_FN(NAME)(&SM_CTX(), &TST_FLAGS(NAME));CHECK_TEST(NAME);

// Weak assertions
#define CHECK(COND) DO_CHECK(CHECK, COND, NOPE)
#define CHECK_EQ(EXPR, EXPECTED) DO_CHECK_EQ(CHECK_EQ, EXPR, EXPECTED, NOPE)
#define CHECK_NEQ(EXPR, EXPECTED) DO_CHECK_NEQ(CHECK_NEQ, EXPR, EXPECTED, NOPE)
#define CHECK_NULL(EXPR) DO_CHECK_NULL(CHECK_NULL, EXPR, NOPE)
#define CHECK_VALUE(EXPR) DO_CHECK_VALUE(CHECK_VALUE, EXPR, NOPE)

// Strong assertions
#define REQUIRE(COND) DO_CHECK(REQUIRED, COND, STOP)
#define REQUIRE_EQ(EXPR, EXPECTED) DO_CHECK_EQ(REQUIRE_EQ, EXPR, EXPECTED, STOP)
#define REQUIRE_NEQ(EXPR, EXPECTED) DO_CHECK_NEQ(REQUIRE_NEQ, EXPR, EXPECTED, STOP)
#define REQUIRE_NULL(EXPR) DO_CHECK_NULL(REQUIRE_NULL, EXPR, STOP)
#define REQUIRE_VALUE(EXPR) DO_CHECK_VALUE(REQUIRE_VALUE, EXPR, STOP)
