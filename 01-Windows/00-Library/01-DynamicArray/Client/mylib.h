
#define SUCCESS                     1
#define ARRAY_INDEX_OUT_OF_BOUNDS   2

#define ARR_SIZE 10

typedef int ret_t;

extern "C" struct dynamic_array {
    int* p_arr;
    int  size;
};

extern "C" struct dynamic_array* create_dynamic_array(int arr_size);
extern "C" int get_element_at_index(struct dynamic_array* p_dynamic_arr, int index, int* p_element);
extern "C" int set_element_at_index(struct dynamic_array* p_dynamic_arr, int index, int p_new_element);
extern "C" int destroy_dynamic_array(struct dynamic_array* p_dynamic_arr);
