#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 8080

struct Student
{
    char login_id[50];
    char password[50];
    char name[100];
    int age;
    char email_id[100];
    char address[100];
};

// General Functionality
void hide_password(char *password, int max_length)
{
    struct termios old_term, new_term;
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    int i = 0;
    char ch;

    while (1)
    {
        ch = getchar();
        if (ch == '\n' || i >= max_length - 1)
        {
            break;
        }
        password[i] = ch;
        i++;
    }

    password[i] = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
    printf("\n");
}

void writeStudentToFile(struct Student student, const char *filename)
{
    int file_fd = open(filename, O_WRONLY | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return;
    }

    char buffer[512];
    snprintf(buffer, sizeof(buffer), "%s$%s$%s$%d$%s$%s\n",
             student.login_id,
             student.password,
             student.name,
             student.age,
             student.email_id,
             student.address);

    write(file_fd, buffer, strlen(buffer));
    close(file_fd);
}

void readStudentsFromFile(const char *filename)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return;
    }

    char buffer[512];
    ssize_t bytesRead;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *token = strtok(buffer, ",");
        struct Student student;

        if (token != NULL)
        {
            strncpy(student.login_id, token, sizeof(student.login_id) - 1);
            token = strtok(NULL, ",");
        }

        if (token != NULL)
        {
            strncpy(student.password, token, sizeof(student.password) - 1);
            token = strtok(NULL, ",");
        }

        if (token != NULL)
        {
            strncpy(student.name, token, sizeof(student.name) - 1);
            token = strtok(NULL, ",");
        }

        if (token != NULL)
        {
            student.age = atoi(token);
            token = strtok(NULL, ",");
        }

        if (token != NULL)
        {
            strncpy(student.email_id, token, sizeof(student.email_id) - 1);
            token = strtok(NULL, ",");
        }

        if (token != NULL)
        {
            strncpy(student.address, token, sizeof(student.address) - 1);
        }
    }

    close(file_fd);
}

// Student Functionality
void show_student_menu()
{
    printf("1) Enroll to new Courses\n2) Unenroll from already enrolled Courses\n3) View enrolled Courses\n4) Password Change\n5) Exit\n");
}

// Fauctly Functionality
void show_faculty_menu()
{
    printf("1) Add new Course\n2) Remove offered Course\n3) View enrollments in Courses\n4) Password Change\n5) Exit\n");
}

// Admin Functionality
void show_admin_menu()
{
    printf("1) Add student\n2) Add Faculty\n3) Activate/Deactivate Student\n4) Update Student/Faculty details\n5) Exit\n");
}

int main()
{

    // DO NOT TOUCH CODE START
    int client_socket;
    struct sockaddr_in server_addr;
    int auth_status, change_pass_status;
    char username[50], password[50], curr_password[50], new_password[50];
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
        // DO NOT TOUCH CODE END
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
                printf("Enter Current Password: ");
                scanf("%s", curr_password);
                printf("Enter New Password: ");
                scanf("%s", new_password);
                send(client_socket, &choice, sizeof(int), 0);
                send(client_socket, curr_password, sizeof(curr_password), 0);
                send(client_socket, new_password, sizeof(new_password), 0);
                recv(client_socket, &change_pass_status, sizeof(int), 0);
                if (change_pass_status == 1)
                {
                    printf("succ");
                }
                else
                {
                    printf("again");
                }
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
            // Add student
            if (choice == 1)
            {
                struct Student student_info;
                getchar();
                printf("Enter Roll No/Log in ID: ");
                scanf("%[^\n]", student_info.login_id);
                getchar();
                printf("Enter Password: ");
                scanf("%[^\n]", student_info.password);
                getchar();
                printf("Enter Name: ");
                scanf("%[^\n]", student_info.name);
                getchar();
                printf("Enter age: ");
                scanf("%d", &student_info.age);
                getchar();
                printf("Enter email ID: ");
                scanf("%[^\n]", student_info.email_id);
                getchar();
                printf("Enter Address: ");
                scanf("%[^\n]", student_info.address);
                getchar();
                // Write student data to the file
                // writeStudentToFile(student_info, "data/students_data/student_data.txt");

                // int add_std = 1;
                send(client_socket,&choice,sizeof(int),0);
                
                char buffer[sizeof(struct Student)];
                memcpy(buffer, &student_info, sizeof(struct Student));
                
                send(client_socket, buffer, sizeof(struct Student), 0);

            }
            // Add Faculty
            else if (choice == 2)
            {
            }
            // Activate/Deactivate Student
            else if (choice == 3)
            {
            }
            // Update Student/Faculty details
            else if (choice == 4)
            {
            }
            // Exit
            else if (choice == 5)
            {
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