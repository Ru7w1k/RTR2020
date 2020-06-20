#include <stdio.h>
#include <string.h>

#define NAME_LENGTH    100
#define MARITAL_STATUS 10

// defining struct
struct Employee
{
    char name[NAME_LENGTH];
    int age;
    float salary;
    char sex;
    char marital_status[MARITAL_STATUS];
};

int main(void)
{
    // variable declarations
    struct Employee EmployeeRecord_rc[5]; // an array of 5 structs: Each being type 'struct Employee'

    char employee_rajesh_rc[] = "Rajesh";
    char employee_sameer_rc[] = "Sameer";
    char employee_kalyani_rc[] = "Kalyani";
    char employee_sonali_rc[] = "Sonali";
    char employee_shantanu_rc[] = "Shantanu";

    int i_rc;
    
    // code
    // HARD-CODED INITIALIZATION OF ARRAY OF struct Employee

    // EMPLOYEE 1
    strcpy(EmployeeRecord_rc[0].name, employee_rajesh_rc);
    EmployeeRecord_rc[0].age = 30;
    EmployeeRecord_rc[0].sex = 'M';
    EmployeeRecord_rc[0].salary = 50000.0f;
    strcpy(EmployeeRecord_rc[0].marital_status, "Unmarried");

    // EMPLOYEE 2
    strcpy(EmployeeRecord_rc[1].name, employee_sameer_rc);
    EmployeeRecord_rc[1].age = 32;
    EmployeeRecord_rc[1].sex = 'M';
    EmployeeRecord_rc[1].salary = 60000.0f;
    strcpy(EmployeeRecord_rc[1].marital_status, "Married");

    // EMPLOYEE 3
    strcpy(EmployeeRecord_rc[2].name, employee_kalyani_rc);
    EmployeeRecord_rc[2].age = 29;
    EmployeeRecord_rc[2].sex = 'F';
    EmployeeRecord_rc[2].salary = 62000.0f;
    strcpy(EmployeeRecord_rc[2].marital_status, "Unmarried");

    // EMPLOYEE 4
    strcpy(EmployeeRecord_rc[3].name, employee_sonali_rc);
    EmployeeRecord_rc[3].age = 33;
    EmployeeRecord_rc[3].sex = 'F';
    EmployeeRecord_rc[3].salary = 50000.0f;
    strcpy(EmployeeRecord_rc[3].marital_status, "Married");

    // EMPLOYEE 5
    strcpy(EmployeeRecord_rc[4].name, employee_shantanu_rc);
    EmployeeRecord_rc[4].age = 35;
    EmployeeRecord_rc[4].sex = 'M';
    EmployeeRecord_rc[4].salary = 55000.0f;
    strcpy(EmployeeRecord_rc[4].marital_status, "Married");

    // DISPLAY
    printf("\n\n");
    printf("DISPLAYING EMPLOYEE RECORDS\n\n");
    for (i_rc = 0; i_rc < 5; i_rc++)
    {
        printf("EMPLOYEE NUMBER %d\n\n", (i_rc + 1));
        printf("Name           : %s\n", EmployeeRecord_rc[i_rc].name);
        printf("Age            : %d years\n", EmployeeRecord_rc[i_rc].age);

        if (EmployeeRecord_rc[i_rc].sex == 'M' || EmployeeRecord_rc[i_rc].sex == 'm')
            printf("Sex            : Male\n");
        else
            printf("Sex            : Female\n");

        printf("Salary         : Rs. %f\n", EmployeeRecord_rc[i_rc].salary);
        printf("Marital Status : %s\n", EmployeeRecord_rc[i_rc].marital_status);
        
        printf("\n\n");
    }

    return(0);
}
