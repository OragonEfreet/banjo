// BjForwardList is included in forward_list.h
#include <banjo/forward_list.h>

#include <banjo/log.h>

// A forward list is a linear container where each element is linked to its next one.

int main(int argc, char* argv[]) {

    
    // Usual Creation and destruction
    BjForwardListInfo create_info = { .value_size = sizeof(int) };
    BjForwardList list = bj_forward_list_create(&create_info, 0);
    
    // Initially, a list is 0 length
    bj_log(INFO, "Initial count: %d", bj_forward_list_count(list));

    // Items can be appended to list end

    bj_forward_list_destroy(list);
    

    return 0;
}

