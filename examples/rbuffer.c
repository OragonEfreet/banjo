////////////////////////////////////////////////////////////////////////////////
/// \example rbuffer.c
/// Demonstrates how to use the \ref bj_rbuffer container, the bucket-based
/// ring buffer.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/api.h>
#include <banjo/rbuffer.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#include <stdlib.h>
#include <stdio.h>


int main(void) {





    return 0;
}




/* void test_null_safety() { */
/*     REQUIRE(bj_rbuffer_ready(NULL) == 0); */
/*     REQUIRE(bj_rbuffer_available(NULL) == 0); */
/*     REQUIRE(bj_rbuffer_push(NULL, 0, 5) == BJ_FALSE); */
/*     REQUIRE(bj_rbuffer_pop(NULL, 0, 5) == BJ_FALSE); */
/*     REQUIRE(bj_rbuffer_reserve(NULL, 1000) == 0); */
/*     bj_rbuffer_reset(NULL); */
/*     REQUIRE(bj_rbuffer_capacity(NULL) == 0); */
/* } */
