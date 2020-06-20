#include <stdio.h>
#include <ctype.h>

#define NUM_EMPLOYEE   5 // simply change this constant value to have as many number of Employee Records as you please...

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
    struct Employee EmployeeRecord_rc[NUM_EMPLOYEE]; // an array of <NUM_EMPLOYEES> structs: Each being type 'struct Employee'
    int i_rc;
    
    // code
    // USER INPUT INITIALIZATION OF ARRAY OF struct Employee
    for (i_rc = 0; i_rc < NUM_EMPLOYEE; i_rc++)
    {
        printf("\n\n\n\n");
        printf("DATA ENTRY FOR EMPLOYEE NUMBER %d\n\n", i_rc+1);

        printf("\n\n");
        printf("Enter Employee Name: ");
        MyGetString(EmployeeRecord_rc[i_rc].name, NAME_LENGTH);

        printf("\n\n\n\n");
        printf("Enter Employee's Age (in years): ");
        scanf("%d", &EmployeeRecord_rc[i_rc].age);

        printf("\n\n");
        printf("Enter Employee's Sex (M/m for Male, F/f for Female): ");
        EmployeeRecord_rc[i_rc].sex = getch();
        printf("%c", EmployeeRecord_rc[i_rc].sex);
        EmployeeRecord_rc[i_rc].sex = toupper(EmployeeRecord_rc[i_rc].sex);

        printf("\n\n\n\n");
        printf("Enter Employee's Salary (in Indian Rupees): ");
        scanf("%f", &EmployeeRecord_rc[i_rc].salary);

        printf("\n\n");
        printf("Is the Employee Married? (Y/y for Yes, N/n for No): ");
        EmployeeRecord_rc[i_rc].marital_status = getch();
        printf("%c", EmployeeRecord_rc[i_rc].marital_status);
        EmployeeRecord_rc[i_rc].marital_status = toupper(EmployeeRecord_rc[i_rc].marital_status);

    }

    // DISPLAY
    printf("\n\n\n\n");
    printf("DISPLAYING EMPLOYEE RECORDS\n\n");
    for (i_rc = 0; i_rc < NUM_EMPLOYEE; i_rc++)
    {
        printf("EMPLOYEE NUMBER %d\n\n", (i_rc + 1));
        printf("Name           : %s\n", EmployeeRecord_rc[i_rc].name);
        printf("Age            : %d years\n", EmployeeRecord_rc[i_rc].age);

        if (EmployeeRecord_rc[i_rc].sex == 'M')
            printf("Sex            : Male\n");
        else if (EmployeeRecord_rc[i_rc].sex == 'F')
            printf("Sex            : Female\n");
        else
            printf("Sex            : Invalid Data Entered\n");

        printf("Salary         : Rs. %f\n", EmployeeRecord_rc[i_rc].salary);

        if (EmployeeRecord_rc[i_rc].marital_status == 'Y')
            printf("Marital Status : Married\n");
        else if (EmployeeRecord_rc[i_rc].marital_status == 'N')
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


