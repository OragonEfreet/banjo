#pragma once

#include "mock_memory.h"

#include <stdbool.h>
#include <ctype.h>
#include <string.h>

// Symbol overrides
#define SM_NS(SM) test_ ## SM
#define SM_TST_FN(NAME) NAME
#define TST_FLAGS(NAME) SM_NS(flags_ ## NAME) 
#define TST_STRUCT_T(NAME) struct SM_NS(data_ ## NAME) 
#define SM_CTX() SM_NS(context) 
#define SM_FLAGS_PARAM() SM_NS(flags) 
#define SM_DATA_PARAM() SM_NS(data) 

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
    const char*           prog_name;   // Name of the curernt test progrm
    unsigned char         n_run;       // Number of test cases already finished
    unsigned char         n_fail;      // Number of failed test cases
    size_t                 n_asserts;   // Total number of checked assertions
    int                   stop_at_err; // If 0, program continues on error
    void*                 user_data;   // User data
    sAllocationData       allocations; // Allocation Data
} Context;

// Parses ARGC/ARGV and builds the Context object
void initialize_context(Context* context, int argc, char* argv[]) {
    bj_memset(context, 0, sizeof(Context));
    if(argc >= 1) {
        context->prog_name = argv[0];
    } else {
        context->prog_name = "UnitTest";
    }

    bj_memory_callbacks allocators = mock_allocators(&context->allocations);
    bj_memory_set_defaults(&allocators);
}



// Called after each test ends
// Returns 1 means "terminate program"
int record_test_result(Context* context, const char* test_name, int status_flag) {
    ++context->n_run;
    if ((status_flag & FAIL) > 0) {
        ++context->n_fail;
        PRINT("[FAIL] | %s\n\n", test_name);
        return 1 & context->stop_at_err;
    }
    PRINT("[OK]   | %s\n", test_name);
    return 0;
}

// Terminate the test program, outputs a report
// Return 0 means all test passed
int terminate_context(Context* SM_CTX()) {
    const char* fmt = SM_CTX()->stop_at_err ? "Ran up to %d test cases, %d asserts.\n%d test failed\n" : "Ran %d test cases, %d asserts.\n%d test failed.\n";
    PRINT(fmt, SM_CTX()->n_run, SM_CTX()->n_asserts, SM_CTX()->n_fail);

    if(SM_CTX()->n_fail == 0) {
            PRINT("\nApplication allocated:\t %zu (max: %zu)\nTotal allocated:\t %zu (max %zu)\n%d allocs, %d reallocs, %d frees\n",
            SM_CTX()->allocations.application_current_allocated, SM_CTX()->allocations.application_max_allocated,
            SM_CTX()->allocations.actual_current_allocated, SM_CTX()->allocations.actual_max_allocated,
            SM_CTX()->allocations.n_allocations, SM_CTX()->allocations.n_reallocations, SM_CTX()->allocations.n_free
        );
        if(SM_CTX()->allocations.application_current_allocated > 0) {
            PRINT("Potential leaks detected\n");
            return -1;
        } else {
            PRINT("No leak detected\n");
            return 0;
        }
    }

    return SM_CTX()->n_fail;
}

bool all_zero(void* ptr, size_t byte_size) {
    char* b = ptr;
    while(byte_size-- > 0) {
        if((*b++) != 0) {
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// INTERNAL MACROS
#define DEFINE_TST_FN(NAME) void SM_TST_FN(NAME)(Context* SM_CTX(), StatusFlag* SM_FLAGS_PARAM())
#define DEFINE_TST_FN_ARGS(NAME) void SM_TST_FN(NAME)(Context* SM_CTX(), StatusFlag* SM_FLAGS_PARAM(), TST_STRUCT_T(NAME)* SM_DATA_PARAM())
#define RESET_TST_FLAGS(NAME) TST_FLAGS(NAME) = 0
#define DECL_TST_FLAGS(NAME) static StatusFlag RESET_TST_FLAGS(NAME)
#define DECL_TST_DATASTRUCT(NAME, CONTENT) TST_STRUCT_T(NAME) CONTENT

#define TRACE(NAME, STR, ...) PRINT("         %s:%d: FAILED (%s): " STR "\n", SM_CTX()->prog_name, __LINE__, #NAME, __VA_ARGS__);
#define CHECK_TEST(NAME, LABEL) if (record_test_result(&SM_CTX(), LABEL, TST_FLAGS(NAME))) END_TESTS();

#define NOPE ;
#define STOP *SM_FLAGS_PARAM() |= FAIL;return;

#define DO_CHECK(CMD, COND, IF_FAIL) if(!(COND)) { TRACE(CMD, "%s", #COND); IF_FAIL; } else { ++SM_CTX()->n_asserts;}
#define DO_CHECK_EQ(CMD, RES, EXPECTED, IF_FAIL)  if(!(RES == EXPECTED)) { TRACE(CMD, "%s: expected %s", #RES, #EXPECTED); IF_FAIL; }  else { ++SM_CTX()->n_asserts;}
#define DO_CHECK_NEQ(CMD, RES, EXPECTED, IF_FAIL)  if((RES == EXPECTED)) { TRACE(CMD, "%s: expected NOT %s", #RES, #EXPECTED); IF_FAIL; }  else { ++SM_CTX()->n_asserts;}
#define DO_CHECK_NULL(CMD, RES, IF_FAIL)  if(!(RES == 0)) { TRACE(CMD, "%s", #RES); IF_FAIL; }  else { ++SM_CTX()->n_asserts;}
#define DO_CHECK_VALUE(CMD, RES, IF_FAIL)  if((RES == 0)) { TRACE(CMD, "%s", #RES); IF_FAIL; }  else { ++SM_CTX()->n_asserts;}
#define DO_CHECK_EMPTY(CMD, T, OBJ, IF_FAIL) if(!all_zero(OBJ, sizeof(T))) { TRACE(CMD, "%s object not empty", #T); IF_FAIL; }  else { ++SM_CTX()->n_asserts;}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC
#define BEGIN_TESTS(ARGC, ARGV) Context SM_CTX(); initialize_context(&SM_CTX(), ARGC, ARGV)
#define END_TESTS() return terminate_context(&SM_CTX())

#define TEST_CASE_ARGS(NAME, DATA) DECL_TST_DATASTRUCT(NAME, DATA); DECL_TST_FLAGS(NAME); DEFINE_TST_FN_ARGS(NAME)
#define TEST_CASE(NAME) DECL_TST_FLAGS(NAME); DEFINE_TST_FN(NAME)

#define RUN_TEST_ARGS(NAME, ...) RESET_TST_FLAGS(NAME); SM_TST_FN(NAME)(&SM_CTX(), &TST_FLAGS(NAME), &(TST_STRUCT_T(NAME)){__VA_ARGS__});CHECK_TEST(NAME, #NAME " {" #__VA_ARGS__ "}");//PRINT("%s\n", #__VA_ARGS__);
#define RUN_TEST(NAME) RESET_TST_FLAGS(NAME); SM_TST_FN(NAME)(&SM_CTX(), &TST_FLAGS(NAME));CHECK_TEST(NAME, #NAME);


// Weak assertions
#define CHECK(COND) DO_CHECK(CHECK, COND, NOPE)
#define CHECK_EQ(EXPR, EXPECTED) DO_CHECK_EQ(CHECK_EQ, EXPR, EXPECTED, NOPE)
#define CHECK_NEQ(EXPR, EXPECTED) DO_CHECK_NEQ(CHECK_NEQ, EXPR, EXPECTED, NOPE)
#define CHECK_NULL(EXPR) DO_CHECK_NULL(CHECK_NULL, EXPR, NOPE)
#define CHECK_VALUE(EXPR) DO_CHECK_VALUE(CHECK_VALUE, EXPR, NOPE)
#define CHECK_EMPTY(T, OBJ) DO_CHECK_EMPTY(CHECK_EMPTY, T, OBJ, NOPE)

// Strong assertions
#define REQUIRE(COND) DO_CHECK(REQUIRED, COND, STOP)
#define REQUIRE_EQ(EXPR, EXPECTED) DO_CHECK_EQ(REQUIRE_EQ, EXPR, EXPECTED, STOP)
#define REQUIRE_NEQ(EXPR, EXPECTED) DO_CHECK_NEQ(REQUIRE_NEQ, EXPR, EXPECTED, STOP)
#define REQUIRE_NULL(EXPR) DO_CHECK_NULL(REQUIRE_NULL, EXPR, STOP)
#define REQUIRE_VALUE(EXPR) DO_CHECK_VALUE(REQUIRE_VALUE, EXPR, STOP)
#define REQUIRE_NIL(T, OBJ) DO_CHECK_EMPTY(REQUIRE_NIL, T, OBJ, STOP)
