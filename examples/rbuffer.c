////////////////////////////////////////////////////////////////////////////////
/// \example rbuffer.c
/// Demonstrates how to use the \ref bj_rbuffer container, the bucket-based
/// ring buffer.
////////////////////////////////////////////////////////////////////////////////
#include <banjo/api.h>
#include <banjo/rbuffer.h>
#include <banjo/log.h>
#include <banjo/memory.h>

#define BUCKET_SIZE 16

int main(void) {

    bj_rbuffer* rbuffer = bj_rbuffer_new_t(int, BUCKET_SIZE);


    for(size_t c = 1 ; c < (BUCKET_SIZE + 1) ; ++c) {
        const size_t req_cap = (c * (BUCKET_SIZE + 1)) - (BUCKET_SIZE / 2);

        const size_t act_cap = bj_rbuffer_reserve(rbuffer, req_cap);

        bj_info("Reserved for %d items, got %d", req_cap, act_cap);

    }
    


    bj_rbuffer_del(rbuffer);
}
