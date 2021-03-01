//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>

#include "mylib.h"
#pragma comment(lib, "mylib.lib")

/* test_dynamic_array.c */
int main(void)
{
    struct dynamic_array* p_dynamic_array = NULL;
    int i;
    int data;
    ret_t return_value;

    printf("TEST");

    p_dynamic_array = create_dynamic_array(ARR_SIZE);
    printf("TEST");
    for (i = 0; i < ARR_SIZE; i++) {
        return_value = set_element_at_index(p_dynamic_array, i, (i+1)*10);
        assert(return_value == SUCCESS);
    }

    for (i = 0; i < ARR_SIZE; i++) {
        return_value = get_element_at_index(p_dynamic_array, i, &data);
        assert(return_value == SUCCESS);
        printf("Data at index %d: %d\n", i, data);
    }

    return_value = destroy_dynamic_array(p_dynamic_array);
    assert(return_value == SUCCESS);
    p_dynamic_array = NULL;

    return (EXIT_SUCCESS);
}
