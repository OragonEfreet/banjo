// BjForwardList is included in forward_list.h
#include <banjo/forward_list.h>

#include <banjo/log.h>

// A forward list is a linear container where each element is linked to its next one.

int main(int argc, char* argv[]) {

    
    // Usual Creation and destruction
    BjForwardListCreateInfo create_info = { .elem_size = sizeof(int) };
    BjForwardList list                  = BJ_NULL_HANDLE;
    bjCreateForwardList(&create_info, &list);
    
    // Initially, a list is 0 length
    bjLog(INFO, "Initial count: %d", bjForwardListCount(list));

    // Items can be appended to list end

    bjDestroyForwardList(list);
    

    return 0;
}
