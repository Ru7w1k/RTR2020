//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <cassert>

#include "mylib.h"

// BOOL DllMain(HMODULE hDll, DWORD dwReason, LPVOID reserver)
// {
//     return TRUE;
// }

extern "C" struct dynamic_array* create_dynamic_array(int arr_size) {
    struct dynamic_array* p_dynamic_array = NULL;

    p_dynamic_array = (struct dynamic_array*)malloc(sizeof(struct dynamic_array));

    if (p_dynamic_array == NULL) {
        printf("Insufficient Memory!\n");
        exit(-1);
    }

    if (arr_size < 0) {
        puts("Bad size for array!");
        exit(-1);
    }

    p_dynamic_array->size = arr_size;
    p_dynamic_array->p_arr = (int*)malloc(arr_size * sizeof(int));
    if (p_dynamic_array->p_arr == NULL) {
        puts("Bad size for array!");
        exit(-1);
    }

    return (p_dynamic_array);
}

extern "C" int get_element_at_index(struct dynamic_array* p_dynamic_arr, int index, int* p_element) {
    if (index < 0 || index >= p_dynamic_arr->size)
        return ARRAY_INDEX_OUT_OF_BOUNDS;

    *p_element = p_dynamic_arr->p_arr[index];
    return (SUCCESS);
}

extern "C" int set_element_at_index(struct dynamic_array* p_dynamic_arr, int index, int p_new_element) {
    if (index < 0 || index >= p_dynamic_arr->size)
        return ARRAY_INDEX_OUT_OF_BOUNDS;

    p_dynamic_arr->p_arr[index] = p_new_element;
    return (SUCCESS);

}

extern "C" int destroy_dynamic_array(struct dynamic_array* p_dynamic_arr) {
    free(p_dynamic_arr->p_arr);
    free(p_dynamic_arr);
    return (SUCCESS);
}

