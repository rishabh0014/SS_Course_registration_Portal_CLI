#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>

#define PORT 8080

void show_student_menu()
{
    printf("1) Enroll to new Courses\n2) Unenroll from already enrolled Courses\n3) View enrolled Courses\n4) Password Change\n5) Exit\n");
}

void show_faculty_menu()
{
    printf("1) Add new Course\n2) Remove offered Course\n3) View enrollments in Courses\n4) Password Change\n5) Exit\n");
}

void show_admin_menu()
{
    printf("1) Add student\n2) Add Faculty\n3) Activate/Deactivate Student\n4) Update Student/Faculty details\n5) Exit\n");
}

int main()
{

    // DO NOT TOUCH CODE START
    int client_socket;
    struct sockaddr_in server_addr;
    int auth_status;
    char username[50], password[50];
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Error in socket creation");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error in connection");
        exit(1);
    }

    printf("Hello! Welcome to the Course Registration Portal\n");
    int num;

    // choose role
    while (1)
    {
        printf("Log in As:\n 1) Student\n 2) Faculty\n 3) Admin\n 4) Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &num);
        if (num < 1 || 4 < num)
        {
            printf("Invalid Choice!! Try Again");
        }
        else
        {
            break;
        }
    }
    if (num == 4)
    {
        printf("Bye Bye!!\n");
        printf("Exiting...!!\n");
        exit(0);
    }
    // DO NOT TOUCH CODE END

    printf("Enter username: ");
    scanf("%s", username);
    getchar();
    printf("Enter password: ");
    hide_password(password, sizeof(password));

    send(client_socket, &num, sizeof(int), 0);
    send(client_socket, username, sizeof(username), 0);
    send(client_socket, password, sizeof(password), 0);
    recv(client_socket, &auth_status, sizeof(int), 0);

    if (auth_status == 1)
    {
        printf("Authentication successful!\n");
        printf("Hello, %s! Welcome..\n", username);
        // student logic
        if (num == 1)
        {
            int choice;
            while (1)
            {
                show_student_menu();
                scanf("%d", &choice);
                if (choice < 1 || 5 < choice)
                {
                    printf("Invalid choice.. Try Again...\n");
                }
                else
                {
                    break;
                }
            }

            if (choice == 1)
            {
                // Enroll to new Courses
            }
            else if (choice == 2)
            {
                // Unenroll from already enrolled Courses
            }
            else if (choice == 3)
            {
                // View enrolled Courses
            }
            else if (choice == 4)
            {
                // Password Change
            }
            else if (choice == 5)
            {
                // Exit
                printf("Signing Out... BYE BYE!!");
                return 0;
            }
        }
        // faculty logic
        else if (num == 2)
        {
            int choice;
            while (1)
            {
                show_faculty_menu();
                scanf("%d", &choice);
                if (choice < 1 || 5 < choice)
                {
                    printf("Invalid choice.. Try Again...\n");
                }
                else
                {
                    break;
                }
            }
            if (choice == 1)
            {
                // Add new Course
            }
            else if (choice == 2)
            {
                // Remove offered Course
            }
            else if (choice == 3)
            {
                // View enrollments in Courses
            }
            else if (choice == 4)
            {
                // Password Change
            }
            else if (choice == 5)
            {
                // Exit
                printf("Signing Out... BYE BYE!!");
                return 0;
            }
        }
        // admin logic
        else if (num == 3)
        {
            int choice;
            while (1)
            {
                show_admin_menu();
                scanf("%d", &choice);
                if (choice < 1 || 5 < choice)
                {
                    printf("Invalid choice.. Try Again...\n");
                }
                else
                {
                    break;
                }
            }
            if (choice == 1)
            {
                // Add student
            }
            else if (choice == 2)
            {
                // Add Faculty
            }
            else if (choice == 3)
            {
                // Activate/Deactivate Student
            }
            else if (choice == 4)
            {
                // Update Student/Faculty details
            }
            else if (choice == 5)
            {
                // Exit
                printf("Signing Out... BYE BYE!!");
                return 0;
            }
        }
    }
    else
    {
        printf("Authentication failed.\n");
    }
    return 0;
}