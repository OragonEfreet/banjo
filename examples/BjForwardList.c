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

    // Let's append some values
    for(int i = 0 ; i < 10 ; i ++) {
        bj_forward_list_prepend(list, &i);
    }

    bj_log(INFO, "Total count: %d", bj_forward_list_count(list));

    BjForwardListIterator it = bj_forward_list_iterator_create(list);

    do {
        bj_info("%d", *(int*)bj_forward_list_iterator_value(it));
    } while(bj_forward_list_iterator_next(it));


    // Items can be appended to list end

    bj_forward_list_destroy(list);
    

    return 0;
}

