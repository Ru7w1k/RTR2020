#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* dynamic_array.h */
/* header file -> 1 */
#define SUCCESS                     1
#define ARRAY_INDEX_OUT_OF_BOUNDS   2

#define ARR_SIZE 10

typedef int ret_t;

/* header file -> 2 */
struct dynamic_array {
    int* p_arr;
    int  size;
};

/* header file -> 3 */
struct dynamic_array* create_dynamic_array(int arr_size);
int get_element_at_index(struct dynamic_array* p_dynamic_arr, int index, int* p_element);
int set_element_at_index(struct dynamic_array* p_dynamic_arr, int index, int p_new_element);
int destroy_dynamic_array(struct dynamic_array* p_dynamic_arr);

/* test_dynamic_array.c */
int main(void)
{
    struct dynamic_array* p_dynamic_array = NULL;
    int i;
    int data;
    ret_t return_value;

    p_dynamic_array = create_dynamic_array(ARR_SIZE);
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

/* dynamic_array.c */
struct dynamic_array* create_dynamic_array(int arr_size) {
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

int get_element_at_index(struct dynamic_array* p_dynamic_arr, int index, int* p_element) {
    if (index < 0 || index >= p_dynamic_arr->size)
        return ARRAY_INDEX_OUT_OF_BOUNDS;

    *p_element = p_dynamic_arr->p_arr[index];
    return (SUCCESS);
}

int set_element_at_index(struct dynamic_array* p_dynamic_arr, int index, int p_new_element) {
    if (index < 0 || index >= p_dynamic_arr->size)
        return ARRAY_INDEX_OUT_OF_BOUNDS;

    p_dynamic_arr->p_arr[index] = p_new_element;
    return (SUCCESS);

}

int destroy_dynamic_array(struct dynamic_array* p_dynamic_arr) {
    free(p_dynamic_arr->p_arr);
    free(p_dynamic_arr);
    return (SUCCESS);
}


