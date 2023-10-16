#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <termios.h>

#define PORT 8080
#define MAX_CLIENTS 10
// #define MAX_COURSES 50
#define MAX_STUDENTS 100
#define MAX_COURSES 5
#define MAX_LINE_LENGTH 450

int clients_count = 0;

struct Student
{
    char login_id[50];
    char password[50];
    char name[100];
    char age[10];
    char email_id[100];
    char address[100];
    char activate_stu[5];
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

struct Course
{
    char course_id[20];
    char course_name[30];
    char faculty_id[50];
    char max_seats[20];
    char rem_seats[20];
};

struct Student_Courses
{
    char login_id[50];
    char taken_course[20];
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
    snprintf(buffer, sizeof(buffer), "%s$%s$%s$%s$%s$%s$%s$\n",
             student.login_id,
             student.password,
             student.name,
             student.age,
             student.email_id,
             student.address,
             student.activate_stu);

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
    snprintf(buffer, sizeof(buffer), "%s$%s$%s\n", student.login_id, student.password, student.activate_stu);
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
    char temp[5] = "1";
    snprintf(buffer, sizeof(buffer), "%s$%s$%s$\n", faculty.login_id, faculty.password, temp);
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
            char stored_username[50], stored_password[50], stored_activate_stu[5];

            sscanf(line, "%49[^$]$%49[^$]$%4[^$]", stored_username, stored_password, stored_activate_stu);

            if (strcmp(stored_username, username) == 0 && strcmp(stored_password, password) == 0)
            {
                if (strcmp(stored_activate_stu, "1") == 0)
                {
                    auth_status = 1;
                    close(file_fd);
                    return auth_status;
                }
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

            sscanf(line, "%49[^$]$%49[^$]$%99[^$]$%9[^$]$%99[^$]$%99[^$]$%4[^$]", student.login_id, student.password, student.name, student.age, student.email_id, student.address, student.activate_stu);
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

void removeStudentDetails(const char *login_id, const char *filename)
{
    FILE *originalFile = fopen(filename, "r");
    if (originalFile == NULL)
    {
        perror("Error opening file");
        return;
    }

    FILE *tempFile = fopen("tempfile.txt", "w");
    if (tempFile == NULL)
    {
        perror("Error creating temporary file");
        fclose(originalFile);
        return;
    }

    char line[256];
    int found = 0;

    while (fgets(line, sizeof(line), originalFile) != NULL)
    {
        char stored_login_id[50];
        if (sscanf(line, "%49[^$]$", stored_login_id) == 1)
        {
            if (strcmp(stored_login_id, login_id) == 0)
            {
                found = 1;
            }
            else
            {
                fputs(line, tempFile);
            }
        }
    }

    fclose(originalFile);
    fclose(tempFile);

    if (found)
    {
        if (remove(filename) != 0)
        {
            perror("Error removing original file");
        }
        if (rename("tempfile.txt", filename) != 0)
        {
            perror("Error renaming temporary file");
        }
    }
    else
    {
        remove("tempfile.txt");
        printf("No student with login ID %s found.\n", login_id);
    }
}

int update_activation_status_login_file(const char *login_id, const char *new_val, const char *filename)
{
    int file_fd = open(filename, O_RDWR);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }
    char buffer[256];
    char new_file_contents[256] = "";
    ssize_t bytesRead;
    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            char stored_login_id[50], password[50], activate_stu[5];
            sscanf(line, "%49[^$]$%49[^$]$%4[^$]$", stored_login_id, password, activate_stu);
            if (strcmp(stored_login_id, login_id) == 0)
            {
                strncpy(activate_stu, new_val, sizeof(activate_stu));
            }
            char updated_line[256];
            snprintf(updated_line, sizeof(updated_line), "%s$%s$%s$\n", stored_login_id, password, activate_stu);
            strcat(new_file_contents, updated_line);

            line = strtok(NULL, "\n");
        }
    }
    close(file_fd);
    file_fd = open(filename, O_WRONLY | O_TRUNC);
    if (file_fd < 0)
    {
        perror("Error reopening file for writing");
        return 0;
    }
    write(file_fd, new_file_contents, strlen(new_file_contents));
    close(file_fd);
    return 1;
}

int change_password(const char *login_id, const char *old_password, const char *new_password, const char *filename)
{
    int file_fd = open(filename, O_RDWR);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }
    char line[256];
    ssize_t bytesRead;
    int student_modified = 0;
    off_t line_start = 0;
    while ((bytesRead = read(file_fd, line, sizeof(line) - 1)) > 0)
    {
        line[bytesRead] = '\0';
        char *stored_login_id = strtok(line, "$");
        char *password = strtok(NULL, "$");
        char *activation_status = strtok(NULL, "$");
        if (stored_login_id != NULL && password != NULL && activation_status != NULL)
        {
            if (strcmp(stored_login_id, login_id) == 0)
            {
                if (strcmp(password, old_password) == 0)
                {
                    off_t new_line_start = lseek(file_fd, 0, SEEK_CUR) - bytesRead;
                    lseek(file_fd, line_start, SEEK_SET);
                    char updated_line[256];
                    snprintf(updated_line, sizeof(updated_line), "%s$%s$%s$", stored_login_id, new_password, activation_status);
                    write(file_fd, updated_line, strlen(updated_line));
                    lseek(file_fd, new_line_start, SEEK_SET);
                    student_modified = 1;
                    break;
                }
            }
        }

        line_start = lseek(file_fd, 0, SEEK_CUR);
    }

    close(file_fd);

    return student_modified;
}

int update_student_details(const char *login_id, const char *this_detail, const char *new_data, const char *filename)
{
    struct Student result;
    struct Student new_student;
    if (search_student_by_id(login_id, filename, &result))
    {
        strcpy(new_student.login_id, result.login_id);
        strcpy(new_student.password, result.password);
        strcpy(new_student.name, result.name);
        strcpy(new_student.age, result.age);
        strcpy(new_student.email_id, result.email_id);
        strcpy(new_student.address, result.address);
        strcpy(new_student.activate_stu, result.activate_stu);
        removeStudentDetails(login_id, filename);
        if (strcmp(this_detail, "login id") == 0)
        {
            write_student_data_to_file(new_student, filename);
            return 0;
        }
        else if (strcmp(this_detail, "password") == 0)
        {
            strcpy(new_student.password, new_data);
            change_password(login_id, result.password, new_data, filename);
        }
        else if (strcmp(this_detail, "name") == 0)
        {
            strcpy(new_student.name, new_data);
        }
        else if (strcmp(this_detail, "age") == 0)
        {
            strcpy(new_student.age, new_data);
        }
        else if (strcmp(this_detail, "email") == 0)
        {
            strcpy(new_student.email_id, new_data);
        }
        else if (strcmp(this_detail, "address") == 0)
        {
            strcpy(new_student.address, new_data);
        }
        else if (strcmp(this_detail, "status") == 0)
        {
            printf("%s", new_data);
            update_activation_status_login_file(login_id, new_data, "data/students_data/student_log_in.txt");
            strcpy(new_student.activate_stu, new_data);
        }
        else
        {
            return 0;
        }
        write_student_data_to_file(new_student, filename);
        return 1;
    }
    else
    {
        return 0;
    }
}

int update_faculty_details(const char *login_id, const char *this_detail, const char *new_data, const char *filename)
{
    struct Faculty result;
    struct Faculty new_faculty;

    if (search_faculty_by_id(login_id, filename, &result))
    {
        strcpy(new_faculty.login_id, result.login_id);
        strcpy(new_faculty.password, result.password);
        strcpy(new_faculty.name, result.name);
        strcpy(new_faculty.department, result.department);
        strcpy(new_faculty.designation, result.designation);
        strcpy(new_faculty.email_id, result.email_id);
        strcpy(new_faculty.address, result.address);
        removeStudentDetails(login_id, filename);
        if (strcmp(this_detail, "login id") == 0)
        {
            write_faculty_data_to_file(new_faculty, filename);
            return 0;
        }
        else if (strcmp(this_detail, "password") == 0)
        {
            // call modify password;
            strcpy(new_faculty.password, new_data);
        }
        else if (strcmp(this_detail, "name") == 0)
        {
            strcpy(new_faculty.name, new_data);
        }
        else if (strcmp(this_detail, "department") == 0)
        {
            strcpy(new_faculty.department, new_data);
        }
        else if (strcmp(this_detail, "designation") == 0)
        {
            strcpy(new_faculty.designation, new_data);
        }
        else if (strcmp(this_detail, "email") == 0)
        {
            strcpy(new_faculty.email_id, new_data);
        }
        else if (strcmp(this_detail, "address") == 0)
        {
            strcpy(new_faculty.address, new_data);
        }
        else
        {
            return 0;
        }
        write_faculty_data_to_file(new_faculty, filename);
        return 1;
    }
    else
    {
        return 0;
    }
}

// Faculty part start
int write_course_data_to_file(struct Course add_course, const char *filename)
{
    int file_fd = open(filename, O_WRONLY | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }
    int succ = 0;
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "%s$%s$%s$%s$%s$\n",
             add_course.course_id,
             add_course.course_name,
             add_course.faculty_id,
             add_course.max_seats,
             add_course.rem_seats);

    if (write(file_fd, buffer, strlen(buffer)))
    {
        succ = 1;
    }
    close(file_fd);
    return succ;
}

int view_all_courses(struct Course all_courses[], const char *filename)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }

    int count = 0;
    char buffer[256];
    ssize_t bytesRead;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *token = strtok(buffer, "$");
        while (token != NULL)
        {
            strncpy(all_courses[count].course_id, token, sizeof(all_courses[count].course_id) - 1);
            token = strtok(NULL, "$");
            if (token != NULL)
            {
                strncpy(all_courses[count].course_name, token, sizeof(all_courses[count].course_name) - 1);
                token = strtok(NULL, "$");
            }
            if (token != NULL)
            {
                strncpy(all_courses[count].faculty_id, token, sizeof(all_courses[count].faculty_id) - 1);
                token = strtok(NULL, "$");
            }
            if (token != NULL)
            {
                strncpy(all_courses[count].max_seats, token, sizeof(all_courses[count].max_seats) - 1);
                token = strtok(NULL, "$");
            }
            if (token != NULL)
            {
                strncpy(all_courses[count].rem_seats, token, sizeof(all_courses[count].rem_seats) - 1);
                token = strtok(NULL, "$");
            }
            count++;
        }
    }

    close(file_fd);
    return count;
}

int view_offering_courses(struct Course all_courses[], const char *filename, const char *faculty_id)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }

    int count = 0;
    char buffer[256];
    ssize_t bytesRead;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *token = strtok(buffer, "$");
        struct Course curr_course;
        while (token != NULL)
        {
            strncpy(curr_course.course_id, token, sizeof(curr_course.course_id) - 1);
            token = strtok(NULL, "$");
            if (token != NULL)
            {
                strncpy(curr_course.course_name, token, sizeof(curr_course.course_name) - 1);
                token = strtok(NULL, "$");
            }
            if (token != NULL)
            {
                strncpy(curr_course.faculty_id, token, sizeof(curr_course.faculty_id) - 1);
                token = strtok(NULL, "$");
            }
            if (token != NULL)
            {
                strncpy(curr_course.max_seats, token, sizeof(curr_course.max_seats) - 1);
                token = strtok(NULL, "$");
            }
            if (token != NULL)
            {
                strncpy(curr_course.rem_seats, token, sizeof(curr_course.rem_seats) - 1);
                token = strtok(NULL, "$");
            }
            if (strcmp(curr_course.faculty_id, faculty_id) == 0)
            {
                all_courses[count] = curr_course;
                count++;
            }
        }
    }

    close(file_fd);
    return count;
}

int remove_course_from_catalog(const char *take_course_id, const char *filename, const char *faculty_id)
{
    FILE *originalFile = fopen(filename, "r");
    if (originalFile == NULL)
    {
        return 0;
    }

    FILE *tempFile = fopen("tempfile.txt", "w");
    if (tempFile == NULL)
    {
        fclose(originalFile);
        return 0;
    }

    char line[256];
    int found = 0;

    while (fgets(line, sizeof(line), originalFile) != NULL)
    {
        struct Course current_course;
        if (sscanf(line, "%19[^$]$%29[^$]$%49[^$]$%19[^$]$%19[^$]$", current_course.course_id, current_course.course_name, current_course.faculty_id, current_course.max_seats, current_course.rem_seats) == 5)
        {
            if (strcmp(current_course.faculty_id, faculty_id) == 0 && strcmp(current_course.course_id, take_course_id) == 0)
            {
                found = 1;
            }
            else
            {
                fputs(line, tempFile);
            }
        }
    }

    fclose(originalFile);
    fclose(tempFile);

    if (found)
    {
        if (remove(filename) != 0)
        {
            return 0;
        }
        if (rename("tempfile.txt", filename) != 0)
        {
            return 0;
        }
    }
    else
    {
        remove("tempfile.txt");
        return 0;
    }
    return 1;
}

int search_course_by_id(const char *course_id, const char *filename, struct Course *result)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }

    char buffer[256];
    ssize_t bytesRead;
    int course_found = 0;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            struct Course course;

            sscanf(line, "%19[^$]$%29[^$]$%49[^$]$%19[^$]$%19[^$]", course.course_id, course.course_name, course.faculty_id, course.max_seats, course.rem_seats);
            if (strcmp(course.course_id, course_id) == 0)
            {
                *result = course;
                course_found = 1;
                close(file_fd);
                return course_found;
            }
            line = strtok(NULL, "\n");
        }
    }
    close(file_fd);
    return course_found;
}

int update_course_details(const char *course_id, const char *this_detail, const char *new_data, const char *filename, const char *faculty_id)
{

    // struct Course result;
    struct Course new_course;
    if (search_course_by_id(course_id, filename, &new_course))
    {
        if (strcmp(new_course.faculty_id, faculty_id) == 0)
        {
            // strcpy(new_course.course_id, result.course_id);
            // strcpy(new_course.course_name, result.course_name);
            // strcpy(new_course.faculty_id, result.faculty_id);
            // strcpy(new_course.max_seats, result.max_seats);
            // strcpy(new_course.rem_seats, result.rem_seats);
            remove_course_from_catalog(course_id, filename, faculty_id);
            if (strcmp(faculty_id, new_course.faculty_id) == 0)
            {
                if (strcmp(this_detail, "course id") == 0)
                {
                    write_course_data_to_file(new_course, filename);
                    return 0;
                }
                else if (strcmp(this_detail, "course name") == 0)
                {
                    strcpy(new_course.course_name, new_data);
                }
                else if (strcmp(this_detail, "faculty id") == 0)
                {
                    strcpy(new_course.faculty_id, new_data);
                }
                else if (strcmp(this_detail, "maximum seats") == 0)
                {
                    strcpy(new_course.max_seats, new_data);
                }
                else if (strcmp(this_detail, "remaining seats") == 0)
                {
                    strcpy(new_course.rem_seats, new_data);
                }
                else
                {
                    return 0;
                }
            }
            write_course_data_to_file(new_course, filename);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

void also_update_in_course_details(struct Course temp){
    // char*endptr;
    // int new_rem_seats = strtol(temp.rem_seats,&endptr,10);
    // if(new_rem_seats>)
    // new_rem_seats--;
    // if()
}
int enroll_new_course(const char *login_id, const char *course_id)
{
    char course_detail_file[] = "data/courses_data/course_details.txt";
    char course_and_students_file[] = "data/courses_data/course_and_students.txt";
    
    struct Course temp;

    if(search_course_by_id(course_id,course_detail_file,&temp)==0){
        return -3;
    }


    int file_fd = open(course_and_students_file, O_RDWR | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }

    char buffer[120];
    ssize_t bytesRead;
    int cnt = 0;
    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            char stored_login_id[50], stored_course_id[50];

            sscanf(line, "%29[^$]$%29[^$]$", stored_login_id, stored_course_id);

            if (strcmp(stored_login_id, login_id) == 0)
            {
                cnt++;
                if (strcmp(stored_course_id, course_id) == 0)
                {
                    return -2;
                }
            }
            line = strtok(NULL, "\n");
        }
    }
    if (cnt >= 5)
    {
        return -1;
    }
    char to_write[100];
    snprintf(to_write, sizeof(to_write), "%s$%s$\n", login_id, course_id);
    if(write(file_fd, to_write, strlen(to_write))){
        char this_detail[30] ="remaining seats"; 
        also_update_in_course_details(temp);
        return 1;

    }

    close(file_fd);
    return 0;
}
void *handle_client(void *arg)
{
    int client_socket = *((int *)arg);
    char username[50], password[50];
    int num, choice;
    int auth_status = 0;
    int users_count;
    char auth_succ_fail[40];
    {
        char welcome[100] = "Hello! Welcome to the Course Registration Portal\n";
        char select_role[100] = "Log in As:\n 1) Student\n 2) Faculty\n 3) Admin\n 4) Exit\nEnter your Choice: ";

        send(client_socket, welcome, sizeof(welcome), 0);
        send(client_socket, select_role, sizeof(select_role), 0);
        recv(client_socket, &num, sizeof(num), 0);
    }

    if (num == 4)
    {
        char exit_[30] = "Bye Bye!!\nExiting...!!\n";
        send(client_socket, exit_, sizeof(exit_), 0);
    }
    else
    {
        // Username and Password
        {
            char en_username[20] = "Enter username: ";
            char en_password[20] = "Enter password: ";
            send(client_socket, en_username, sizeof(en_username), 0);
            send(client_socket, en_password, sizeof(en_password), 0);
            read(client_socket, username, sizeof(username));
            read(client_socket, password, sizeof(password));
        }

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
                strcpy(auth_succ_fail, "Log in Successful");
                send(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
                char student_menu[200] = "1) View All Courses\n2) Enroll new Cources\n3) Drop Courses\n4) view Enrolled Course Details\n5) Change Password\n6) Logout and Exit\n";
                while (1)
                {
                    send(client_socket, student_menu, sizeof(student_menu), 0);
                    recv(client_socket, &choice, sizeof(int), 0);

                    // View All Courses
                    if (choice == 1)
                    {
                        int ack;
                        recv(client_socket, &ack, sizeof(int), 0);
                        if (ack)
                        {
                            char filename[50] = "data/courses_data/course_details.txt";
                            struct Course all_courses[MAX_COURSES];
                            char list_of_all[30] = "List of all Courses";
                            send(client_socket, list_of_all, sizeof(list_of_all), 0);
                            int total_course = view_all_courses(all_courses, filename);
                            send(client_socket, &total_course, sizeof(int), 0);

                            for (int i = 0; i < total_course - 1; i++)
                            {
                                send(client_socket, &all_courses[i], sizeof(all_courses[i]), 0);
                            }
                        }
                    }
                    // Enroll new Cources
                    else if (choice == 2)
                    {
                        char en_new_course[30] = "Enter Course ID: ";
                        send(client_socket, en_new_course, sizeof(en_new_course), 0);
                        char course_id[20];
                        recv(client_socket, course_id, sizeof(course_id), 0);
                        int x =enroll_new_course(username, course_id); 
                        if (x)
                        {
                            printf("Success\n");
                        }
                        printf("%d\n",x);

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
                        char en_old[30] = "Enter Current Password: ";
                        send(client_socket, en_old, sizeof(en_old), 0);
                        char old_password[50];
                        recv(client_socket, old_password, sizeof(old_password), 0);

                        char en_new[30] = "Enter New Password: ";
                        send(client_socket, en_new, sizeof(en_new), 0);
                        char new_password[50];
                        recv(client_socket, new_password, sizeof(new_password), 0);

                        char pass_update_status[50];
                        char temp_this_detail[30] = "password";

                        if (change_password(username, old_password, new_password, "data/students_data/student_log_in.txt") && update_student_details(username, temp_this_detail, new_password, "data/students_data/student_data.txt"))
                        {
                            strcpy(pass_update_status, "Password Changed Successfully");
                        }
                        else
                        {
                            strcpy(pass_update_status, "Check Your Current Password");
                        }
                        send(client_socket, pass_update_status, sizeof(pass_update_status), 0);
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
                strcpy(auth_succ_fail, "Wrong Login ID/Username or Password!!");
                send(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
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
            if (auth_status == 1)
            {
                strcpy(auth_succ_fail, "Log in Successful");
                send(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
                char faculty_menu[200] = "1) View All Courses\n2) Add new Cources\n3) Remove Courses from Catalog\n4) Update Course Details\n5) Change Password\n6) Logout and Exit\n";
                while (1)
                {
                    send(client_socket, faculty_menu, sizeof(faculty_menu), 0);
                    recv(client_socket, &choice, sizeof(int), 0);

                    // View Offered Courses
                    if (choice == 1)
                    {
                        int ack;
                        recv(client_socket, &ack, sizeof(int), 0);
                        if (ack)
                        {
                            char filename[50] = "data/courses_data/course_details.txt";
                            struct Course all_courses[MAX_COURSES];
                            char list_of_all[30] = "List of all Courses";
                            send(client_socket, list_of_all, sizeof(list_of_all), 0);
                            int total_course = view_offering_courses(all_courses, filename, username);
                            send(client_socket, &total_course, sizeof(int), 0);

                            for (int i = 0; i < total_course - 1; i++)
                            {
                                send(client_socket, &all_courses[i], sizeof(all_courses[i]), 0);
                            }
                        }
                    }
                    // Add new Cources
                    else if (choice == 2)
                    {
                        char add_new_courses[30] = "Enter Course Details.\n";
                        send(client_socket, add_new_courses, sizeof(add_new_courses), 0);

                        char en_course_id[30] = "Enter Course ID: ";
                        send(client_socket, en_course_id, sizeof(en_course_id), 0);
                        char new_course_id[20];
                        recv(client_socket, new_course_id, sizeof(new_course_id), 0);

                        char en_course_name[30] = "Enter Course Name: ";
                        send(client_socket, en_course_name, sizeof(en_course_name), 0);
                        char new_course_name[30];
                        recv(client_socket, new_course_name, sizeof(new_course_name), 0);

                        // char en_faculty_id[30] = "Enter Faculty ID: ";
                        // send(client_socket, en_faculty_id, sizeof(en_faculty_id), 0);
                        // char new_faculty_id[50];
                        // recv(client_socket, new_faculty_id, sizeof(new_faculty_id), 0);

                        char en_number_seats[30] = "Enter Number of Seats: ";
                        send(client_socket, en_number_seats, sizeof(en_number_seats), 0);
                        char new_number_seats[20];
                        recv(client_socket, new_number_seats, sizeof(new_number_seats), 0);

                        struct Course add_course;
                        strcpy(add_course.course_id, new_course_id);
                        strcpy(add_course.course_name, new_course_name);
                        strcpy(add_course.faculty_id, username);
                        strcpy(add_course.max_seats, new_number_seats);
                        strcpy(add_course.rem_seats, new_number_seats);

                        char filename[] = "data/courses_data/course_details.txt";
                        char added_succ[30];
                        if (write_course_data_to_file(add_course, filename))
                        {
                            strcpy(added_succ, "Course added successfully");
                        }
                        else
                        {
                            strcpy(added_succ, "Can not add course");
                        }
                        send(client_socket, added_succ, sizeof(added_succ), 0);
                    }
                    // Remove Courses from Catalog
                    else if (choice == 3)
                    {
                        char en_course_id[30] = "Enter Course ID: ";
                        send(client_socket, en_course_id, sizeof(en_course_id), 0);
                        char take_course_id[20];
                        recv(client_socket, take_course_id, sizeof(take_course_id), 0);
                        char filename[] = "data/courses_data/course_details.txt";
                        char remove_status[30];
                        if (remove_course_from_catalog(take_course_id, filename, username))
                        {
                            strcpy(remove_status, "Course removed successfully");
                        }
                        else
                        {
                            strcpy(remove_status, "Can not remove course");
                        }
                        send(client_socket, remove_status, sizeof(remove_status), 0);
                    }
                    // Update Course Details
                    else if (choice == 4)
                    {
                        char en_login[45] = "Enter Course ID: ";
                        send(client_socket, en_login, sizeof(en_login), 0);
                        char course_id[50];
                        recv(client_socket, course_id, sizeof(course_id), 0);

                        char which_det[50] = "Which detail you want to modify? ";
                        send(client_socket, which_det, sizeof(which_det), 0);
                        char this_detail[15];
                        recv(client_socket, this_detail, sizeof(this_detail), 0);
                        char en_nw_det[20] = "Enter new Value: ";
                        send(client_socket, en_nw_det, sizeof(en_nw_det), 0);
                        char new_data[120];
                        recv(client_socket, new_data, sizeof(new_data), 0);
                        char filename[50] = "data/courses_data/course_details.txt";
                        if (update_course_details(course_id, this_detail, new_data, filename, username))
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
                    // Change Password
                    else if (choice == 5)
                    {
                        char en_old[30] = "Enter Current Password: ";
                        send(client_socket, en_old, sizeof(en_old), 0);
                        char old_password[50];
                        recv(client_socket, old_password, sizeof(old_password), 0);

                        char en_new[30] = "Enter New Password: ";
                        send(client_socket, en_new, sizeof(en_new), 0);
                        char new_password[50];
                        recv(client_socket, new_password, sizeof(new_password), 0);

                        char pass_update_status[50];
                        char temp_this_detail[30] = "password";
                        if (change_password(username, old_password, new_password, "data/faculties_data/faculties_log_in.txt") && update_student_details(username, temp_this_detail, new_password, "data/faculties_data/faculty_data.txt"))
                        {
                            strcpy(pass_update_status, "Password Changed Successfully");
                        }
                        else
                        {
                            strcpy(pass_update_status, "Check Your Current Password");
                        }
                        send(client_socket, pass_update_status, sizeof(pass_update_status), 0);
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
                strcpy(auth_succ_fail, "Wrong Login ID/Username or Password!!");
                send(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
            }
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
                strcpy(auth_succ_fail, "Log in Successful");
                send(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
                char admin_menu[200] = "1) Add student\n2) View Student Details\n3) Add Faculty\n4) View Faculty Details\n5) Activate Student\n6) Block Student\n7) Modify Student Details\n8) Modify Faculty Details\n9) Exit\nEnter Your Choice: ";
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
                        char en_stu_id[25] = "Enter Student ID: ";
                        send(client_socket, en_stu_id, sizeof(en_stu_id), 0);
                        char activate_stu_id[50];
                        recv(client_socket, activate_stu_id, sizeof(activate_stu_id), 0);
                        char activation_flag[40];
                        char temp_buff[5] = "1";
                        char temp_buff_status[20] = "account status";
                        if (update_student_details(activate_stu_id, temp_buff_status, temp_buff, "data/students_data/student_data.txt"))
                        {
                            strcpy(activation_flag, "Student account Activated Successfully");
                        }
                        else
                        {
                            strcpy(activation_flag, "Student ID not found");
                        }
                        send(client_socket, activation_flag, sizeof(activation_flag), 0);
                    }
                    // Block Student
                    else if (choice == 6)
                    {
                        char en_stu_id[25] = "Enter Student ID: ";
                        send(client_socket, en_stu_id, sizeof(en_stu_id), 0);
                        char block_stu_id[50];
                        recv(client_socket, block_stu_id, sizeof(block_stu_id), 0);
                        char activation_flag[40];
                        char temp_buff[5] = "0";
                        if (update_student_details(block_stu_id, "account status", temp_buff, "data/students_data/student_data.txt"))
                        {
                            strcpy(activation_flag, "Student account Blocked Successfully");
                        }
                        else
                        {
                            strcpy(activation_flag, "Student ID not found");
                        }
                        send(client_socket, activation_flag, sizeof(activation_flag), 0);
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
                        if (update_student_details(log_in_id, this_detail, new_data, "data/students_data/student_data.txt"))
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
                        char en_login[45] = "Enter Log in ID of the Faculty: ";
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

                        if (update_faculty_details(log_in_id, this_detail, new_data, "data/faculties_data/faculty_data.txt"))
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
                    // LogOut and Exit
                    else if (choice == 9)
                    {
                        break;
                    }
                }
            }
            else
            {
                strcpy(auth_succ_fail, "Wrong Login ID/Username or Password!!");
                send(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
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