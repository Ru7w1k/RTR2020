#include <stdio.h>
#include <ctype.h>

#define NAME_LENGTH    100
#define MARITAL_STATUS 10

// defining struct
struct Employee
{
    char name[NAME_LENGTH];
    int age;
    float salary;
    char sex;
    char marital_status;
};

int main(void)
{
    // function prototype
    void MyGetString(char[], int);

    // variable declarations
    struct Employee *pEmployeeRecord_rc = NULL; 
    int num_emp_rc, i_rc;
    
    // code
    printf("\n\n");
    printf("Enter number of employees whose details you want to record: ");
    scanf("%d", &num_emp_rc);

    printf("\n\n");
    pEmployeeRecord_rc = (struct Employee *)malloc(sizeof(struct Employee) * num_emp_rc);
    if (pEmployeeRecord_rc == NULL)
    {
        printf("FAILED TO ALLOCATE MEMORY FOR %d EMPLOYEES! EXITING NOW..\n\n", num_emp_rc);
        exit(0);
    }
    else
        printf("SUCCESSFULLY ALLOCATED MEMORY FOR %d EMPLOYEES!\n\n", num_emp_rc);

    // USER INPUT INITIALIZATION OF ARRAY OF struct Employee
    for (i_rc = 0; i_rc < num_emp_rc; i_rc++)
    {
        printf("\n\n\n\n");
        printf("DATA ENTRY FOR EMPLOYEE NUMBER %d\n\n", i_rc+1);

        printf("\n\n");
        printf("Enter Employee Name: ");
        MyGetString(pEmployeeRecord_rc[i_rc].name, NAME_LENGTH);

        printf("\n\n\n\n");
        printf("Enter Employee's Age (in years): ");
        scanf("%d", &pEmployeeRecord_rc[i_rc].age);

        printf("\n\n");
        printf("Enter Employee's Sex (M/m for Male, F/f for Female): ");
        pEmployeeRecord_rc[i_rc].sex = getch();
        printf("%c", pEmployeeRecord_rc[i_rc].sex);
        pEmployeeRecord_rc[i_rc].sex = toupper(pEmployeeRecord_rc[i_rc].sex);

        printf("\n\n\n\n");
        printf("Enter Employee's Salary (in Indian Rupees): ");
        scanf("%f", &pEmployeeRecord_rc[i_rc].salary);

        printf("\n\n");
        printf("Is the Employee Married? (Y/y for Yes, N/n for No): ");
        pEmployeeRecord_rc[i_rc].marital_status = getch();
        printf("%c", pEmployeeRecord_rc[i_rc].marital_status);
        pEmployeeRecord_rc[i_rc].marital_status = toupper(pEmployeeRecord_rc[i_rc].marital_status);

    }

    // DISPLAY
    printf("\n\n\n\n");
    printf("DISPLAYING EMPLOYEE RECORDS\n\n");
    for (i_rc = 0; i_rc < num_emp_rc; i_rc++)
    {
        printf("EMPLOYEE NUMBER %d\n\n", (i_rc + 1));
        printf("Name           : %s\n", pEmployeeRecord_rc[i_rc].name);
        printf("Age            : %d years\n", pEmployeeRecord_rc[i_rc].age);

        if (pEmployeeRecord_rc[i_rc].sex == 'M')
            printf("Sex            : Male\n");
        else if (pEmployeeRecord_rc[i_rc].sex == 'F')
            printf("Sex            : Female\n");
        else
            printf("Sex            : Invalid Data Entered\n");

        printf("Salary         : Rs. %f\n", pEmployeeRecord_rc[i_rc].salary);

        if (pEmployeeRecord_rc[i_rc].marital_status == 'Y')
            printf("Marital Status : Married\n");
        else if (pEmployeeRecord_rc[i_rc].marital_status == 'N')
            printf("Marital Status : Unmarried\n");
        else
            printf("Marital Status : Invalid Data Entered\n");
        
        printf("\n\n");
    }

    return(0);
}

// SIMPLE RUDIMENTARY IMPLEMENTATION OF gets_s()
// IMPLEMENTED DUE TO DIFFERENT BEHAVIOUR OF gets_s() / fgets() / fscanf() ON DIFFERENT PLATFORMS
// BACKSPACE/CHARACTER DELETION AND ARROW KEY CURSOR MOVEMENT NOT IMPLEMENTED

void MyGetString(char str[], int str_size)
{
    // variable declarations
    int i_rc;
    char ch_rc = '\0';

    // code
    i_rc = 0;
    do
    {
        ch_rc = getch();
        str[i_rc] = ch_rc;
        printf("%c", str[i_rc]);
        i_rc++;
    } while ((ch_rc != '\r') && (i_rc < str_size));

    if (i_rc == str_size)
        str[i_rc - 1] = '\0';
    else
        str[i_rc] = '\0';
}


