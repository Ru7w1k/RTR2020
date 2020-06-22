#include <stdio.h>

int main(void)
{
    // variable declarations
    int num_rc;
    int *ptr_rc = NULL;
    int *copy_ptr_rc = NULL;

    // code
    num_rc = 5;
    ptr_rc = &num_rc;

    printf("\n\n");
    printf("BEFORE copy_ptr = ptr\n\n");
    printf("  num = %d\n", num_rc);
    printf("  &num = %p\n", &num_rc);
    printf("  *(&num) = %d\n", *(&num_rc));
    printf("  ptr = %p\n", ptr_rc);
    printf("  *ptr = %d\n", *ptr_rc);

    // ptr is an integer pointer variable, that is can hold the address of any integer variable
    // copy_ptr is another integer pointer
    // if ptr = &num ... ptr will contain address of integer variable num
    // if ptr is assigned to copy_ptr, copy_ptr will also contain address of integer variable num
    // hence, now, both ptr and copy ptr will point to num

    copy_ptr_rc = ptr_rc;

    printf("\n\n");

    printf("AFTER copy_ptr = ptr\n\n");
    printf("  num = %d\n", num_rc);
    printf("  &num = %p\n", &num_rc);
    printf("  *(&num) = %d\n", *(&num_rc));
    printf("  ptr = %p\n", ptr_rc);
    printf("  *ptr = %d\n", *ptr_rc);
    printf("  copy_ptr = %p\n", copy_ptr_rc);
    printf("  *copy_ptr = %d\n", *copy_ptr_rc);

    return(0);
}