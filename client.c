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

int main()
{
    int client_socket, num;
    struct sockaddr_in server_addr;
    int auth_status, change_pass_status;
    char username[50], password[50], curr_password[50], new_password[50], welcome[100], select_role[100], en_username[20], en_password[20];

    // socket
    {
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
    }
    // welcome
    {
        recv(client_socket, welcome, sizeof(welcome), 0);
        printf("%s", welcome);
        recv(client_socket, select_role, sizeof(select_role), 0);
        printf("%s", select_role);
    }
    // Choice
    {
        scanf("%d", &num);
        send(client_socket, &num, sizeof(int), 0);
    }
    // Exit
    if (num == 4)
    {
        char exit_[30];
        recv(client_socket, exit_, sizeof(exit_), 0);
        printf("%s", exit_);
        exit(0);
    }
    // Student Faculty Admin
    else
    {
        // Input from User (username and password)
        {
            recv(client_socket, en_username, sizeof(en_username), 0);
            printf("%s", en_username);
            scanf("%s", username);
            getchar();
            recv(client_socket, en_password, sizeof(en_password), 0);
            printf("%s", en_password);
            hide_password(password, sizeof(password));
            send(client_socket, username, sizeof(username), 0);
            send(client_socket, password, sizeof(password), 0);
        }

        // student
        if (num == 1)
        {
            recv(client_socket, &auth_status, sizeof(int), 0);
            if (auth_status == 1)
            {
                printf("Authentication successful!\n");
                printf("Hello, %s! Welcome..\n", username);
                char student_menu[200];
                recv(client_socket, student_menu, sizeof(student_menu), 0);
                printf("%s", student_menu);
                int choice;
                scanf("%d", &choice);
                send(client_socket, &choice, sizeof(int), 0);

                // Enroll to new Courses
                if (choice == 1)
                {
                }
                // Unenroll from already enrolled Courses
                else if (choice == 2)
                {
                }
                // View enrolled Courses
                else if (choice == 3)
                {
                }
                // Password Change
                else if (choice == 4)
                {
                    // printf("Enter Current Password: ");
                    // scanf("%s", curr_password);
                    // printf("Enter New Password: ");
                    // scanf("%s", new_password);
                    // send(client_socket, &choice, sizeof(int), 0);
                    // send(client_socket, curr_password, sizeof(curr_password), 0);
                    // send(client_socket, new_password, sizeof(new_password), 0);
                    // recv(client_socket, &change_pass_status, sizeof(int), 0);
                    // if (change_pass_status == 1)
                    // {
                    //     printf("succ");
                    // }
                    // else
                    // {
                    //     printf("again");
                    // }
                }
                // Exit
                else if (choice == 5)
                {
                    printf("Signing Out... BYE BYE!!");
                    return 0;
                }
            }
            else
            {
                printf("Authentication failed.\n");
            }
        }
        // faculty
        else if (num == 2)
        {
        }
        // admin
        else if (num == 3)
        {
            recv(client_socket, &auth_status, sizeof(int), 0);
            if (auth_status == 1)
            {
                printf("Authentication successful!\n");
                printf("Hello, %s! Welcome..\n", username);
                char admin_menu[200];
                recv(client_socket, admin_menu, sizeof(admin_menu), 0);
                printf("%s", admin_menu);
                int choice;
                scanf("%d", &choice);
                send(client_socket, &choice, sizeof(choice), 0);

                // Add Student
                if (choice == 1)
                {
                    struct Student student_info;
                    // Taking student Info
                    {
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
                        char buffer[sizeof(struct Student)];
                        memcpy(buffer, &student_info, sizeof(struct Student));
                        send(client_socket, buffer, sizeof(struct Student), 0);
                    }
                }
                // View Student Details
                else if (choice == 2)
                {
                    char give_stu_id[100];
                    recv(client_socket, give_stu_id, sizeof(give_stu_id), 0);
                    printf("%s", give_stu_id);
                    char find_login_id[50];
                    scanf("%s", find_login_id);
                    send(client_socket, find_login_id, sizeof(find_login_id), 0);
                    int stu_found;
                    recv(client_socket, &stu_found, sizeof(int), 0);
                    if (stu_found)
                    {
                        char buffer[sizeof(struct Student)];
                        recv(client_socket, buffer, sizeof(struct Student), 0);
                        struct Student student_info;
                        memcpy(&student_info, buffer, sizeof(struct Student));
                        printf("Student Details\n");
                        printf("Student Roll No/Log in ID: %s\n",student_info.login_id);
                        printf("Student Password: %s\n",student_info.password);
                        printf("Student Name: %s\n",student_info.name);
                        printf("Student age: %d\n",student_info.age);
                        printf("Student Email ID: %s\n",student_info.email_id);
                        printf("Student Address: %s\n",student_info.address);
                    }
                    else
                    {
                        char not_found[30];
                        recv(client_socket,not_found,sizeof(not_found),0);
                        printf("%s\n",not_found);
                    }
                }
                // Add Faculty
                else if (choice == 3)
                {
                }
                // View Faculty Details
                else if (choice == 4)
                {
                }
                // Activate Student
                else if (choice == 5)
                {
                }
                // Block Student
                else if (choice == 6)
                {
                }
                // Modify Student Details
                else if (choice == 7)
                {
                }
                // Moify Faculty Details
                else if (choice == 8)
                {
                }
                // LogOut and Exit
                else if (choice == 9)
                {
                }
            }
            else
            {
                printf("Authentication failed.\n");
            }
        }
    }

    return 0;
}