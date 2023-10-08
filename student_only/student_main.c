#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int main(int argc, char *argv[])
{

    // printf("In Student\n");
    // - Enroll to new Courses
    // - Unenroll from already enrolled Courses
    // - View enrolled Courses
    // - Password Change
    // - Exit
    
    printf("Hello, <name>! Welcome..\n");
    int num;
    while (1)
    {
        printf("1) Enroll to new Courses\n2) Unenroll from already enrolled Courses\n3) View enrolled Courses\n4) Password Change\n5) Exit\n");
        scanf("%d", &num);
        if (num < 1 || 5 < num)
        {
            printf("Invalid choice.. Try Again...\n");
        }
        else
        {
            break;
        }
    }

    if (num == 1)
    {
        // enroll new courses
    }
    else if (num == 2)
    {
        // Unenroll from already enrolled courses
    }
    else if (num == 3)
    {
        // view enrolled courses
    }
    else if (num == 4)
    {
        // password change
    }
    else if (num == 5)
    {
        printf("Please wait.... Signing out..\n");
        exit(0);
    }
    return 0;
}