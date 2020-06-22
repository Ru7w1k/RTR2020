#include <stdio.h>

int main(void)
{
    // variable declarations
    int num_rc;
    int *ptr_rc = NULL;
    int ans_rc;

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

    printf("\n\n");

    // add 10 to ptr which is the address of num
    // hence, 10 will be added to the address of num and resultant address will be displayed
    printf("Answer of (ptr + 10) = %p\n", ptr_rc + 10);

    // add 10 to ptr which is the address of num and give value at new address
    // hence, 10 will be added to the address of num and value at resultant address will be displayed
    printf("Answer of *(ptr + 10) = %d\n", *(ptr_rc + 10));
    
    // add 10 to *ptr which is the value at address num
    // hence 10 will be added to the *ptr (num = 5) and the resultant value will be given (*ptr + 10) = num + 10 = 5 + 10 = 15
    printf("Answer of (*ptr + 10) = %d\n", (*ptr_rc + 10));

    // associativity of * (value at address) and ++ and -- operators is from right to left

    // (Right to Left) consider value *ptr ... preincrement *ptr ... that is value at address 'ptr' i.e. *ptr is preincremented
    ++*ptr_rc;
    printf("Answer of ++*ptr: %d\n", *ptr_rc); // brackets not required for pre-increment / pre-decrement

    // (Right to Left) post-increment address of ptr ... that is, address 'ptr' i.e. ptr is post-incremented (ptr++) and then value at new address is displayed (*ptr++)
    *ptr_rc++; // incorrect method of post incrementing a value using pointer
    printf("Answer of *ptr++: %d\n", *ptr_rc++); // brackets ARE required for post-increment / post-decrement

    // (Right to Left) post-increment value *ptr ... that is, value at address ptr, i.e. *ptr is post-incremented (*ptr++)
    ptr_rc = &num_rc;
    (*ptr_rc)++; // correct method of post incrementing a value using pointer...*ptr is 6 ... at this statement, at this statement *ptr remains 6 but at the next statement *ptr = 7 (post increment)
    printf("Answer of (*ptr)++: %d\n", *ptr_rc); // brackets ARE required for post-increment / post-decrement

    return(0);
}