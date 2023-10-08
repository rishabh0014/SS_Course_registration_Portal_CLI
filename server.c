#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#define PORT 8080
#define MAX_CLIENTS 10
int clients_count = 0;

struct User
{
    char username[50];
    char password[50];
    int num;
};

struct Student
{
    char login_id[50];
    char password[50];
    char name[100];
    int age;
    char email_id[100];
    char address[100];
};

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

int readUsersFromFile(struct User users[], const char *filename)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }
    int count = 0;
    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *token = strtok(buffer, " \n");
        while (token != NULL)
        {
            strcpy(users[count].username, token);
            token = strtok(NULL, " \n");
            if (token != NULL)
            {
                strcpy(users[count].password, token);
                token = strtok(NULL, " \n");
                count++;
            }
        }
    }
    close(file_fd);
    return count;
}

int changePassword(struct User users[], const char *username, const char *current_password, const char *new_password)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (strcmp(username, users[i].username) == 0)
        {
            if (strcmp(current_password, users[i].password) == 0)
            {
                strcpy(users[i].password, new_password);
                return 1;
            }
        }
    }
    return 0;
}

void *handle_client(void *arg)
{
    int client_socket = *((int *)arg);
    char username[50], password[50];
    char curr_password[50], new_password[50];
    int num, choice;
    int auth_status = 0, change_pass_status = 0;
    struct User users[MAX_CLIENTS];
    int users_count;

    read(client_socket, &num, sizeof(int));

    read(client_socket, username, sizeof(username));
    read(client_socket, password, sizeof(password));
    if (num == 1)
    { // student Logic
        users_count = readUsersFromFile(users, "data/students_data/student_log_in.txt");
        for (int i = 0; i < users_count; i++)
        {
            if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0)
            {
                auth_status = 1;
                break;
            }
        }
        send(client_socket, &auth_status, sizeof(int), 0);

        read(client_socket, &choice, sizeof(int));
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
            read(client_socket, curr_password, sizeof(curr_password));
            read(client_socket, new_password, sizeof(new_password));

            if (changePassword(users, username, curr_password, new_password))
            {
                FILE *file = fopen("data/students_data/student_log_in.txt", "w");
                if (file != NULL)
                {
                    for (int j = 0; j < users_count; j++)
                    {
                        fprintf(file, "%s %s\n", users[j].username, users[j].password);
                    }
                    fclose(file);
                }
                else
                {
                    perror("Error opening user data file for update");
                }
                change_pass_status = 1;
            }
            send(client_socket, &change_pass_status, sizeof(int), 0);
        }
    }

    // LAST
    else if (num == 2)
    {
        // faculty logic
        users_count = readUsersFromFile(users, "data/faculties_data/faculties_log_in.txt");
        for (int i = 0; i < users_count; i++)
        {
            if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0)
            {
                auth_status = 1;
                break;
            }
        }
        send(client_socket, &auth_status, sizeof(int), 0);
    }
    else if (num == 3)
    {
        // admin logic
        users_count = readUsersFromFile(users, "data/admins_data/admin_log_in.txt");
        for (int i = 0; i < users_count; i++)
        {
            if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0)
            {
                auth_status = 1;
                break;
            }
        }
        send(client_socket, &auth_status, sizeof(int), 0);

        read(client_socket, &choice, sizeof(int));
        if (choice == 1)
        {
            // Add Student
            char buffer[sizeof(struct Student)];
            recv(client_socket, buffer, sizeof(struct Student), 0);
            struct Student student_info;
            memcpy(&student_info, buffer, sizeof(struct Student));
            printf("%s",student_info.name);
            writeStudentToFile(student_info, "data/students_data/student_data.txt");
        }
        else if (choice == 2)
        {
        }
        else if (choice == 3)
        {
        }
        else if (choice == 4)
        {
        }
    }

    close(client_socket);
    clients_count--;
    pthread_exit(NULL);
}

int main()
{
    // DO NOT TOUCH CODE START
    int server_socket, new_socket;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    pthread_t tid[MAX_CLIENTS];
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Error in socket creation");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error in binding");
        exit(1);
    }
    if (listen(server_socket, MAX_CLIENTS) == 0)
    {
        printf("Listening...\n");
    }
    else
    {
        perror("Error in listening");
        exit(1);
    }
    while (1)
    {
        addr_size = sizeof(new_addr);
        new_socket = accept(server_socket, (struct sockaddr *)&new_addr, &addr_size);
        if (pthread_create(&tid[clients_count], NULL, handle_client, &new_socket) != 0)
        {
            perror("Error in creating thread");
        }
        clients_count++;
        printf("%d\n", clients_count);
        if (clients_count >= MAX_CLIENTS)
        {
            printf("Maximum number of clients reached. Connection rejected.\n");
            close(new_socket);
        }
    }
    close(server_socket);
    // DO NOT TOUCH CODE END
}