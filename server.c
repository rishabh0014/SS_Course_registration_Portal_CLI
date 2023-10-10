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

struct Faculty
{
    char login_id[50];
    char password[50];
    char name[100];
    char department[50];
    char designation[50];
    char email_id[100];
    char address[100];
};

void write_student_data_to_file(struct Student student, const char *filename)
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

void write_student_log_in_data_to_file(struct Student student, const char *filename)
{
    int file_fd = open(filename, O_WRONLY | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening File");
        return;
    }

    char buffer[120];
    snprintf(buffer, sizeof(buffer), "%s$%s\n", student.login_id, student.password);
    write(file_fd, buffer, strlen(buffer));
    close(file_fd);
}

void write_faculty_data_to_file(struct Faculty faculty, const char *filename)
{
    int file_fd = open(filename, O_WRONLY | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return;
    }

    char buffer[512];
    snprintf(buffer, sizeof(buffer), "%s$%s$%s$%s$%s$%s$%s\n",
             faculty.login_id,
             faculty.password,
             faculty.name,
             faculty.department,
             faculty.designation,
             faculty.email_id,
             faculty.address);

    write(file_fd, buffer, strlen(buffer));
    close(file_fd);
}

void write_faculty_log_in_data_to_file(struct Faculty faculty, const char *filename)
{
    int file_fd = open(filename, O_WRONLY | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening File");
        return;
    }

    char buffer[120];
    snprintf(buffer, sizeof(buffer), "%s$%s\n", faculty.login_id, faculty.password);
    write(file_fd, buffer, strlen(buffer));
    close(file_fd);
}

int readUsersFromFile(const char *username, const char *password, const char *filename)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }

    char buffer[120];
    ssize_t bytesRead;
    int auth_status = 0;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            char stored_username[50], stored_password[50];

            sscanf(line, "%49[^$]$%49[^$]$", stored_username, stored_password);

            if (strcmp(stored_username, username) == 0 && strcmp(stored_password, password) == 0)
            {
                auth_status = 1;
                close(file_fd);
                return auth_status;
            }

            line = strtok(NULL, "\n");
        }
    }

    close(file_fd);
    return auth_status;
}

int search_student_by_id(const char *student_id, const char *filename, struct Student *result)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }

    char buffer[256];
    ssize_t bytesRead;
    int student_found = 0;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            struct Student student;

            sscanf(line, "%49[^$]$%49[^$]$%99[^$]$%d$%99[^$]$%99[^$]", student.login_id, student.password, student.name, &student.age, student.email_id, student.address);

            if (strcmp(student.login_id, student_id) == 0)
            {
                *result = student;
                student_found = 1;
                close(file_fd);
                return student_found;
            }

            line = strtok(NULL, "\n");
        }
    }

    close(file_fd);
    return student_found;
}

int search_faculty_by_id(const char *faculty_id, const char *filename, struct Faculty *result)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }

    char buffer[256];
    ssize_t bytesRead;
    int faculty_found = 0;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            struct Faculty faculty;

            sscanf(line, "%49[^$]$%49[^$]$%99[^$]$%49[^$]$%49[^$]$%99[^$]$%99[^$]", faculty.login_id, faculty.password, faculty.name, faculty.department, faculty.designation, faculty.email_id, faculty.address);

            if (strcmp(faculty.login_id, faculty_id) == 0)
            {
                *result = faculty;
                faculty_found = 1;
                close(file_fd);
                return faculty_found;
            }

            line = strtok(NULL, "\n");
        }
    }

    close(file_fd);
    return faculty_found;
}

int updateStudentDetails(const char *log_in_id, const char *this_detail, const char *new_data, const char *filename)
{
    return 0;
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
    int users_count;

    char welcome[100] = "Hello! Welcome to the Course Registration Portal\n";
    char select_role[100] = "Log in As:\n 1) Student\n 2) Faculty\n 3) Admin\n 4) Exit\nEnter your Choice: ";

    send(client_socket, welcome, sizeof(welcome), 0);
    send(client_socket, select_role, sizeof(select_role), 0);
    recv(client_socket, &num, sizeof(num), 0);

    if (num == 4)
    {
        char exit_[30] = "Bye Bye!!\nExiting...!!\n";
        send(client_socket, exit_, sizeof(exit_), 0);
    }
    else
    {
        char en_username[20] = "Enter username: ";
        char en_password[20] = "Enter password: ";
        send(client_socket, en_username, sizeof(en_username), 0);
        send(client_socket, en_password, sizeof(en_password), 0);
        read(client_socket, username, sizeof(username));
        read(client_socket, password, sizeof(password));
        // student Logic
        if (num == 1)
        {
            if (readUsersFromFile(username, password, "data/students_data/student_log_in.txt"))
            {
                auth_status = 1;
            }
            send(client_socket, &auth_status, sizeof(int), 0);
            if (auth_status == 1)
            {
                char student_menu[200] = "1) Enroll to new Courses\n2) Unenroll from already enrolled Courses\n3) View enrolled Courses\n4) Password Change\n5) Exit\n";
                while (1)
                {
                    send(client_socket, student_menu, sizeof(student_menu), 0);
                    recv(client_socket, &choice, sizeof(int), 0);

                    // View All Courses
                    if (choice == 1)
                    {
                    }
                    // Enroll new Cources
                    else if (choice == 2)
                    {
                    }
                    // Drop Courses
                    else if (choice == 3)
                    {
                    }
                    // View Enrolled Course Details
                    else if (choice == 4)
                    {
                    }
                    // Change Password
                    else if (choice == 5)
                    {
                    }
                    // Logout and Exit
                    else if (choice == 6)
                    {
                        break;
                    }
                }
            }
            else
            {
                // authentication fail
            }
        }
        // faculty logic
        else if (num == 2)
        {
            if (readUsersFromFile(username, password, "data/faculties_data/faculties_log_in.txt"))
            {
                auth_status = 1;
            }
            send(client_socket, &auth_status, sizeof(int), 0);
        }
        // admin logic
        else if (num == 3)
        {
            if (readUsersFromFile(username, password, "data/admins_data/admin_log_in.txt"))
            {
                auth_status = 1;
            }
            send(client_socket, &auth_status, sizeof(int), 0);
            if (auth_status == 1)
            {
                char admin_menu[200] = "1) Add student\n2) View Student Details\n3) Add Faculty\n4) View Faculty Details\n5) Activate Student\n6) Block Student\n7) Modify Student Details\n8) Modify Faculty Details\n9) Exit\n";
                while (1)
                {
                    send(client_socket, admin_menu, sizeof(admin_menu), 0);
                    read(client_socket, &choice, sizeof(int));

                    // Add Studednt
                    if (choice == 1)
                    {
                        char buffer[sizeof(struct Student)];
                        recv(client_socket, buffer, sizeof(struct Student), 0);
                        struct Student student_info;
                        memcpy(&student_info, buffer, sizeof(struct Student));
                        write_student_data_to_file(student_info, "data/students_data/student_data.txt");
                        write_student_log_in_data_to_file(student_info, "data/students_data/student_log_in.txt");
                    }
                    // View Student Details
                    else if (choice == 2)
                    {
                        char give_stu_id[100] = "Enter Student ID: ";
                        send(client_socket, give_stu_id, sizeof(give_stu_id), 0);
                        char find_login_id[50];
                        recv(client_socket, find_login_id, sizeof(find_login_id), 0);
                        struct Student student_info;
                        int stu_found = 0;
                        if (search_student_by_id(find_login_id, "data/students_data/student_data.txt", &student_info))
                        {
                            stu_found = 1;
                            send(client_socket, &stu_found, sizeof(int), 0);
                            char buffer[sizeof(struct Student)];
                            memcpy(buffer, &student_info, sizeof(struct Student));
                            send(client_socket, buffer, sizeof(struct Student), 0);
                        }
                        else
                        {
                            send(client_socket, &stu_found, sizeof(int), 0);
                            char not_found[30] = "Student Not Found";
                            send(client_socket, not_found, sizeof(not_found), 0);
                        }
                    }
                    // Add Faculty
                    else if (choice == 3)
                    {
                        char buffer[sizeof(struct Faculty)];
                        recv(client_socket, buffer, sizeof(struct Faculty), 0);
                        struct Faculty faculty_info;
                        memcpy(&faculty_info, buffer, sizeof(struct Faculty));
                        write_faculty_data_to_file(faculty_info, "data/faculties_data/faculty_data.txt");
                        write_faculty_log_in_data_to_file(faculty_info, "data/faculties_data/faculties_log_in.txt");
                    }
                    // View Faculty Details
                    else if (choice == 4)
                    {
                        char give_fac_id[100] = "Enter Faculty ID: ";
                        send(client_socket, give_fac_id, sizeof(give_fac_id), 0);
                        char find_login_id[50];
                        recv(client_socket, find_login_id, sizeof(find_login_id), 0);
                        struct Faculty faculty_info;
                        int fac_found = 0;
                        if (search_faculty_by_id(find_login_id, "data/faculties_data/faculty_data.txt", &faculty_info))
                        {
                            fac_found = 1;
                            send(client_socket, &fac_found, sizeof(int), 0);
                            char buffer[sizeof(struct Faculty)];
                            memcpy(buffer, &faculty_info, sizeof(struct Faculty));
                            send(client_socket, buffer, sizeof(struct Faculty), 0);
                        }
                        else
                        {
                            send(client_socket, &fac_found, sizeof(int), 0);
                            char not_found[30] = "Faculty Not Found";
                            send(client_socket, not_found, sizeof(not_found), 0);
                        }
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
                        char en_login[45] = "Enter Log in ID of the Student: ";
                        send(client_socket, en_login, sizeof(en_login), 0);
                        char log_in_id[50];
                        recv(client_socket, log_in_id, sizeof(log_in_id), 0);

                        char which_det[50] = "Which detail you want to modify? ";
                        send(client_socket, which_det, sizeof(which_det), 0);
                        char this_detail[15];
                        recv(client_socket, this_detail, sizeof(this_detail), 0);
                        char en_nw_det[20] = "Enter new Value: ";
                        send(client_socket, en_nw_det, sizeof(en_nw_det), 0);
                        char new_data[120];
                        recv(client_socket, new_data, sizeof(new_data), 0);

                        if (updateStudentDetails(log_in_id, this_detail, new_data, "data/students_data/student_data.txt"))
                        {
                            char update_status[40] = "Details Updated Successfully";
                            send(client_socket, update_status, sizeof(update_status), 0);
                        }
                        else
                        {
                            char update_status[40] = "Updation Failed";
                            send(client_socket, update_status, sizeof(update_status), 0);
                        }
                    }
                    // Moify Faculty Details
                    else if (choice == 8)
                    {
                    }
                    // LogOut and Exit
                    else if (choice == 9)
                    {
                        break;
                    }
                }
            }
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