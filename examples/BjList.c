// BjList is included in list.h
#include <banjo/list.h>

#include <banjo/log.h>

typedef struct {float x; float y;} vec2f;


// A forward list is a linear container where each element is linked to its next one.

int main(int argc, char* argv[]) {

    
    // Usual Creation and destruction
    BjListInfo create_info = { .value_size = sizeof(vec2f) };
    BjList* list = bj_list_create(&create_info, 0);
    
    // Initially, a list is 0 length
    bj_log(INFO, "Initial count: %d", bj_list_count(list));

    // Let's append some values
    /* for(int i = 0 ; i < 10 ; i ++) { */
    vec2f* v0 = bj_list_prepend(list, &(vec2f){.x = -1.0f, .y=1.0f});
    vec2f* v1 = bj_list_prepend(list, &(vec2f){.x = 4.4f});
    vec2f* v2 = bj_list_prepend(list, &(vec2f){.x = -2.0f, .y=4.5f});
    /* } */

    bj_log(INFO, "Total count: %d", bj_list_count(list));

    v0 = bj_list_value(list, 2);
    v1 = bj_list_value(list, 1);
    v2 = bj_list_value(list, 0);

    /* BjListIterator it = bj_list_iterator_create(list); */

    /* do { */
    /*     bj_info("%d", *(int*)bj_list_iterator_value(it)); */
    /* } while(bj_list_iterator_next(it)); */


    // Items can be appended to list end

    bj_list_destroy(list);
    

    return 0;
}
