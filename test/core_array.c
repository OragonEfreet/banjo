/* #include <assert.h> */
/* #include <stdlib.h> */
#include <stdio.h>
#include <string.h>

// -----





// Symbol overrides
#define SM_NS(SM) test_ ## SM
#define SM_TST_FN(NAME) SM_NS(fn_ ## NAME)
#define TST_DATA(NAME) SM_NS(data_ ## NAME)
#define SM_CTX() SM_NS(context)
#define SM_DATA_PARAM() SM_NS(data)

// Behaviour overrides
#ifndef PRINT
#   define PRINT(...) printf(__VA_ARGS__)
#endif


typedef struct {
    void* user_data;
} Context;


void initialize_context(Context* SM_CTX(), int argc, char* argv[]) {
}

int terminate_context(Context* SM_CTX()) {
    return 0;
}


typedef struct TestCase_T {
    int res;
    void (*fn)(Context*, struct TestCase_T*);
} TestCase;

void fail_assert(Context* context, struct TestCase_T* test_data) {
    test_data->res = -1;
}



static const char* fname(const char* path) {
#ifdef _MSC_VER
    const char* fptr = strrchr(path, '/');  // For Unix-like paths
#else
    const char* fptr = strrchr(path, '\\'); // For Windows paths
#endif
    if (fptr == 0) { fptr = path; } else { fptr++; }
    return fptr;
}

// INTERNAL
#define DECL_TST_FN(NAME) static void SM_TST_FN(NAME)(Context*, struct TestCase_T*)
#define DEFINE_TST_FN(NAME) void SM_TST_FN(NAME)(Context* SM_CTX(), struct TestCase_T* SM_DATA_PARAM())
#define DECL_TST_DATA(NAME) static TestCase TST_DATA(NAME) = {.res=0,.fn=SM_TST_FN(NAME)}

#define TRACE(COND) PRINT("%s:%d: FAILED: REQUIRE( %s )\n", fname(__FILE__), __LINE__, #COND);
#define ON_FAIL(COND) TRACE(COND); 

// PUBLIC
#define BEGIN_TESTS(ARGC, ARGV) Context SM_CTX() = {}; initialize_context(&SM_CTX(), ARGC, ARGV)
#define END_TESTS() return terminate_context(&SM_CTX())

#define TEST_CASE(NAME) DECL_TST_FN(NAME); DECL_TST_DATA(NAME); DEFINE_TST_FN(NAME)

#define RUN_TEST(NAME) TST_DATA(NAME).fn(&SM_CTX(), &TST_DATA(NAME))

#define REQUIRE(COND) if(!(COND)) {ON_FAIL(COND);fail_assert(SM_CTX(), SM_DATA_PARAM());return;}

// User Code

TEST_CASE(pouet) {
    REQUIRE(4 == 5);
}


int main(int argc, char* argv[]) {

    BEGIN_TESTS(argc, argv);


    RUN_TEST(pouet);
    











    END_TESTS();
}
