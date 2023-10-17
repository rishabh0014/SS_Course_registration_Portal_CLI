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
#define MAX_CLIENTS 20
#define MAX_COURSES 10

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

//Admin part Start
void write_student_data_to_file(struct Student student, const char *filename)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
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
    fcntl(file_fd, F_SETLKW, &lock);

    write(file_fd, buffer, strlen(buffer));
    lock.l_type = F_UNLCK;
    fcntl(file_fd, F_SETLKW, &lock);

    close(file_fd);
}

void write_student_log_in_data_to_file(struct Student student, const char *filename)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    int file_fd = open(filename, O_WRONLY | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening File");
        return;
    }
    char buffer[120];
    snprintf(buffer, sizeof(buffer), "%s$%s$%s\n", student.login_id, student.password, student.activate_stu);
    fcntl(file_fd, F_SETLKW, &lock);
    write(file_fd, buffer, strlen(buffer));
    lock.l_type = F_UNLCK;
    fcntl(file_fd, F_SETLKW, &lock);
    close(file_fd);
}

void write_faculty_data_to_file(struct Faculty faculty, const char *filename)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
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
    fcntl(file_fd, F_SETLKW, &lock);
    write(file_fd, buffer, strlen(buffer));
    lock.l_type = F_UNLCK;
    fcntl(file_fd, F_SETLKW, &lock);

    close(file_fd);
}

void write_faculty_log_in_data_to_file(struct Faculty faculty, const char *filename)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    int file_fd = open(filename, O_WRONLY | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening File");
        return;
    }

    char buffer[120];
    char temp[5] = "1";
    snprintf(buffer, sizeof(buffer), "%s$%s$%s$\n", faculty.login_id, faculty.password, temp);
    fcntl(file_fd, F_SETLKW, &lock);
    write(file_fd, buffer, strlen(buffer));
    lock.l_type = F_UNLCK;
    fcntl(file_fd, F_SETLKW, &lock);
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
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    FILE *originalFile = fopen(filename, "r");

    if (originalFile == NULL)
    {
        perror("Error opening file");
        return;
    }

    FILE *tempFile = fopen("tempfile.txt", "w");
    int file_fd = fileno(tempFile);
    if (tempFile == NULL)
    {
        perror("Error creating temporary file");
        fclose(originalFile);
        return;
    }

    char line[256];
    int found = 0;
    fcntl(file_fd, F_SETLKW, &lock);

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
    lock.l_type = F_UNLCK;
    fcntl(file_fd, F_SETLKW, &lock);

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
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    int file_fd = open(filename, O_RDWR);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }

    char buffer[256];
    ssize_t bytesRead;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            char stored_username[50], stored_password[50], stored_activate_stu[5];

            sscanf(line, "%49[^$]$%49[^$]$%4[^$]$", stored_username, stored_password, stored_activate_stu);
            printf("%s,%s\n", stored_username, stored_password);
            if (strcmp(stored_username, login_id) == 0)
            {
                printf("q");
                removeStudentDetails(login_id, filename);
                struct Student curr_stu;
                strcpy(curr_stu.login_id, login_id);
                strcpy(curr_stu.password, stored_password);
                strcpy(curr_stu.activate_stu, new_val);
                fcntl(file_fd, F_SETLKW, &lock);
                write_student_log_in_data_to_file(curr_stu, filename);
                lock.l_type = F_UNLCK;
                fcntl(file_fd, F_SETLKW, &lock);
                close(file_fd);
                return 1;
            }
            line = strtok(NULL, "\n");
        }
    }
    close(file_fd);
    return 0;
}

int change_password(const char *login_id, const char *old_password, const char *new_password, const char *filename)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return 0;
    }

    char buffer[256];
    ssize_t bytesRead;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            char stored_username[50], stored_password[50], stored_activate_stu[5];

            sscanf(line, "%49[^$]$%49[^$]$%4[^$]", stored_username, stored_password, stored_activate_stu);

            if (strcmp(stored_username, login_id) == 0 && strcmp(stored_password, old_password) == 0)
            {
                removeStudentDetails(login_id, filename);
                struct Student curr_stu;
                strcpy(curr_stu.login_id, login_id);
                strcpy(curr_stu.password, new_password);
                strcpy(curr_stu.activate_stu, stored_activate_stu);
                fcntl(file_fd, F_SETLKW, &lock);
                write_student_log_in_data_to_file(curr_stu, filename);
                lock.l_type = F_UNLCK;
                fcntl(file_fd, F_SETLKW, &lock);
                close(file_fd);
                return 1;
            }
            line = strtok(NULL, "\n");
        }
    }
    close(file_fd);
    return 0;
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
            change_password(login_id, result.password, new_data, "data/students_data/student_log_in.txt");
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
        else if (strcmp(this_detail, "account status") == 0)
        {
            // printf("EE");
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
// Admin Part end

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
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    FILE *originalFile = fopen(filename, "r");
    if (originalFile == NULL)
    {
        return 0;
    }

    FILE *tempFile = fopen("tempfile.txt", "w");
    int file_fd = fileno(tempFile);
    if (tempFile == NULL)
    {
        fclose(originalFile);
        return 0;
    }

    char line[256];
    int found = 0;
    fcntl(file_fd, F_SETLKW, &lock);

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

    lock.l_type = F_UNLCK;
    fcntl(file_fd, F_SETLKW, &lock);
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

    struct Course new_course;
    if (search_course_by_id(course_id, filename, &new_course))
    {
        if (strcmp(new_course.faculty_id, faculty_id) == 0)
        {
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
//Faculty part end

//Student Part start
void update_course_details_by_student(const char *course_id, const char *new_data, const char *filename)
{
    struct Course new_course;
    search_course_by_id(course_id, filename, &new_course);
    remove_course_from_catalog(course_id, filename, new_course.faculty_id);
    strcpy(new_course.rem_seats, new_data);
    write_course_data_to_file(new_course, filename);
}

int is_student_course_exist(const char *login_id, const char *course_id, const char *filename)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error Opening File");
        return -1;
    }

    char buffer[256];
    ssize_t bytesRead;
    int student_found = 0;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            char curr_login_id[50];
            char curr_course_id[20];

            sscanf(line, "%49[^$]$%19[^$]$", curr_login_id, curr_course_id);
            if (strcmp(curr_login_id, login_id) == 0 && strcmp(curr_course_id, course_id) == 0)
            {
                close(file_fd);
                return 1;
            }
            line = strtok(NULL, "\n");
        }
    }
    close(file_fd);
    return 0;
}

int enroll_new_course(const char *login_id, const char *course_id)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    struct Course curr_course;
    if (search_course_by_id(course_id, "data/courses_data/course_details.txt", &curr_course) == 0)
    {
        return -3;
    }

    if (strcmp(curr_course.rem_seats, "0") == 0)
    {
        return -2;
    }
    char *temp;
    int rem_seats = strtol(curr_course.rem_seats, &temp, 10) - 1;
    char new_data[20];
    sprintf(new_data, "%d", rem_seats);
    update_course_details_by_student(course_id, new_data, "data/courses_data/course_details.txt");

    char filename[] = "data/courses_data/course_and_students.txt";
    int file_fd = open(filename, O_RDWR | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return -1;
    }
    char buf[1024];
    if (is_student_course_exist(login_id, course_id, filename) == 1)
    {
        close(file_fd);
        return 0;
    }
    strcpy(buf, login_id);
    strcat(buf, "$");
    strcat(buf, course_id);
    strcat(buf, "$\n");
    printf("%s\n", buf);
    fcntl(file_fd, F_SETLKW, &lock);
    write(file_fd, buf, strlen(buf));
    lock.l_type = F_UNLCK;
    fcntl(file_fd, F_SETLKW, &lock);
    close(file_fd);
    return 1;
}

int remove_course_details_course_stu_using_login_id(const char *login_id, const char *course_id, const char *filename)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    FILE *originalFile = fopen(filename, "r");
    if (originalFile == NULL)
    {
        return 0;
    }

    FILE *tempFile = fopen("tempfile.txt", "w");
    int file_fd = fileno(tempFile);
    if (tempFile == NULL)
    {
        fclose(originalFile);
        return 0;
    }

    char line[256];
    int found = 0;
    fcntl(file_fd, F_SETLKW, &lock);

    while (fgets(line, sizeof(line), originalFile) != NULL)
    {
        char curr_login_id[50];
        char curr_course_id[20];

        if (sscanf(line, "%49[^$]$%19[^$]$", curr_login_id, curr_course_id) == 2)
        {
            if (strcmp(curr_login_id, login_id) == 0 && strcmp(curr_course_id, course_id) == 0)
            {
                found = 1;
            }
            else
            {
                fputs(line, tempFile);
            }
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(file_fd, F_SETLKW, &lock);

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
        return 1;
    }
    return 0;
}

int remove_course_details_course_stu_using_course_id(const char *course_id, const char *filename)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    FILE *originalFile = fopen(filename, "r");
    if (originalFile == NULL)
    {
        return 0;
    }

    FILE *tempFile = fopen("tempfile.txt", "w");
    int file_fd = fileno(tempFile);
    if (tempFile == NULL)
    {
        fclose(originalFile);
        return 0;
    }

    char line[256];
    int found = 0;
    fcntl(file_fd, F_SETLKW, &lock);

    while (fgets(line, sizeof(line), originalFile) != NULL)
    {
        char curr_login_id[50];
        char curr_course_id[20];

        if (sscanf(line, "%49[^$]$%19[^$]$", curr_login_id, curr_course_id) == 2)
        {
            if (strcmp(curr_course_id, course_id) == 0)
            {
                found = 1;
            }
            else
            {
                fputs(line, tempFile);
            }
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(file_fd, F_SETLKW, &lock);
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
        return 1;
    }
    return 0;
}

int drop_new_course(const char *login_id, const char *course_id)
{
    struct Course curr_course;
    if (search_course_by_id(course_id, "data/courses_data/course_details.txt", &curr_course) == 0)
    {
        return -3;
    }

    char filename[] = "data/courses_data/course_and_students.txt";
    int file_fd = open(filename, O_RDWR | O_APPEND);
    if (file_fd < 0)
    {
        perror("Error opening file");
        return -1;
    }

    char buf[1024];
    if (is_student_course_exist(login_id, course_id, filename) == 0)
    {
        close(file_fd);
        return -2;
    }
    if (remove_course_details_course_stu_using_login_id(login_id, course_id, filename) == 1)
    {
        return 0;
    }
    char *temp;
    int rem_seats = strtol(curr_course.rem_seats, &temp, 10) + 1;
    char new_data[20];
    sprintf(new_data, "%d", rem_seats);
    update_course_details_by_student(course_id, new_data, "data/courses_data/course_details.txt");
    return 1;
}

int view_enrolled_course(const char *login_id, const char *filename, int client_socket)
{
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Error Opening File");
        return -1;
    }

    char buffer[256];
    ssize_t bytesRead;
    int student_found = 0;
    int ack = 1;
    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        char *line = strtok(buffer, "\n");
        while (line != NULL)
        {
            char curr_login_id[50];
            char curr_course_id[20];

            sscanf(line, "%49[^$]$%29[^$]$", curr_login_id, curr_course_id);
            if (strcmp(curr_login_id, login_id) == 0)
            {
                struct Course result;
                if (search_course_by_id(curr_course_id, "data/courses_data/course_details.txt", &result) == 1)
                {
                    send(client_socket, &ack, sizeof(int), 0);
                    char buffer[sizeof(struct Course)];
                    memcpy(buffer, &result, sizeof(struct Course));
                    send(client_socket, buffer, sizeof(struct Course), 0);
                    printf("here");
                }
            }
            line = strtok(NULL, "\n");
        }
    }
    // printf("RR");
    ack = 0;
    send(client_socket, &ack, sizeof(int), 0);
    close(file_fd);
    return 0;
}
//Student part end

//New Client part start
void *handle_client(void *arg)
{
    int client_socket = *((int *)arg);
    char username[50], password[50];
    int num, choice;
    int auth_status = 0;
    int users_count;
    char user_type[100];
    char auth_succ_fail[40];

    {
        char welcome[200] = "\n------------------------------Hello! Welcome to the Course Registration Portal------------------------------\n\n";
        char select_role[200] = "Log in As:\n\n 1) Student                    2) Faculty                    3) Admin                    4) Exit\n\nEnter your Choice: ";

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
        if (num == 1)
        {
            strcpy(user_type, "----------------------------------------Student Log In----------------------------------------\n");
        }
        else if (num == 2)
        {
            strcpy(user_type, "----------------------------------------Faculty Log In----------------------------------------\n");
        }
        else if (num == 3)
        {
            strcpy(user_type, "----------------------------------------Admin Log In----------------------------------------\n");
        }
        send(client_socket, user_type, sizeof(user_type), 0);
        // Username and Password
        {
            char en_username[20] = "\nEnter username: ";
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
                strcpy(auth_succ_fail, "\nLog in Successful");
                send(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
                char student_menu[300] = "\nWhat do you want to do?\n\n1) View All Courses\n2) Enroll new Cources\n3) Drop Courses\n4) view Enrolled Course Details\n5) Change Password\n6) Logout and Exit\n\nEnter Your Choice: ";
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
                            char list_of_all[30] = "\nList of all Courses\n";
                            send(client_socket, list_of_all, sizeof(list_of_all), 0);
                            int total_course = view_all_courses(all_courses, filename);
                            send(client_socket, &total_course, sizeof(int), 0);

                            for (int i = 0; i < total_course - 1; i++)
                            {
                                send(client_socket, &all_courses[i], sizeof(all_courses[i]), 0);
                                sleep(1);
                            }
                        }
                    }
                    // Enroll new Cources
                    else if (choice == 2)
                    {
                        char en_course_id[20] = "\nEnter Course ID: ";
                        send(client_socket, en_course_id, sizeof(en_course_id), 0);
                        char course_id[50];
                        recv(client_socket, course_id, sizeof(course_id), 0);
                        int ack = enroll_new_course(username, course_id);
                        char send_status[100];
                        if (ack == 1)
                        {
                            strcpy(send_status, "\nEnrolled Successfully!!!!");
                        }
                        else if (ack == -3)
                        {
                            strcpy(send_status, "\nCourse ID does not Exist");
                        }
                        else if (ack == -2)
                        {
                            strcpy(send_status, "\nMaximum student limit reached!!");
                        }
                        else if (ack == -1)
                        {
                            strcpy(send_status, "\nError in Enrollement!! Try again after sometime");
                        }
                        else
                        {
                            strcpy(send_status, "\nAlready Enrolled!! Can not enrolled again!!");
                        }
                        send(client_socket, send_status, sizeof(send_status), 0);
                    }
                    // Drop Courses
                    else if (choice == 3)
                    {
                        char en_course_id[20] = "\nEnter Course ID: ";
                        send(client_socket, en_course_id, sizeof(en_course_id), 0);
                        char course_id[50];
                        recv(client_socket, course_id, sizeof(course_id), 0);
                        int ack = drop_new_course(username, course_id);
                        printf("ack %d", ack);
                        char send_status[100];
                        if (ack == 1)
                        {
                            strcpy(send_status, "\nCourse Dropped Successfully!!!!");
                        }
                        else if (ack == -3)
                        {
                            strcpy(send_status, "\nCourse ID does not Exist");
                        }
                        else if (ack == -2)
                        {
                            strcpy(send_status, "\nYou did not enroll in this course!!");
                        }
                        else
                        {
                            strcpy(send_status, "\nError!! Try again after sometime");
                        }
                        send(client_socket, send_status, sizeof(send_status), 0);
                    }
                    // View Enrolled Course Details
                    else if (choice == 4)
                    {
                        char filename[50] = "data/courses_data/course_and_students.txt";
                        view_enrolled_course(username, filename, client_socket);
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
                        printf("%s,%s,%s\n", username, old_password, new_password);

                        if (change_password(username, old_password, new_password, "data/students_data/student_log_in.txt") && update_student_details(username, temp_this_detail, new_password, "data/students_data/student_data.txt"))
                        {
                            strcpy(pass_update_status, "\nPassword Changed Successfully");
                        }
                        else
                        {
                            strcpy(pass_update_status, "\nCheck Your Current Password");
                        }
                        send(client_socket, pass_update_status, sizeof(pass_update_status), 0);
                    }
                    // Logout and Exit
                    else if (choice == 6)
                    {
                        char bye_bye[40] = "\nSigning Out.....Please Wait..\n";
                        send(client_socket, bye_bye, sizeof(bye_bye), 0);
                        char bye[20] = "\nBye Bye....\n";
                        send(client_socket, bye, sizeof(bye), 0);
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
                strcpy(auth_succ_fail, "\nLog in Successful");
                send(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
                char faculty_menu[200] = "\nWhat do you want to do?\n\n1) View All Courses\n2) Add new Cources\n3) Remove Courses from Catalog\n4) Update Course Details\n5) Change Password\n6) Logout and Exit\n\nEnter Your Choice: ";
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
                            char list_of_all[30] = "\nList of all Courses";
                            send(client_socket, list_of_all, sizeof(list_of_all), 0);
                            int total_course = view_offering_courses(all_courses, filename, username);
                            send(client_socket, &total_course, sizeof(int), 0);

                            for (int i = 0; i < total_course - 1; i++)
                            {
                                send(client_socket, &all_courses[i], sizeof(all_courses[i]), 0);
                                sleep(1);
                            }
                        }
                    }
                    // Add new Cources
                    else if (choice == 2)
                    {
                        char add_new_courses[30] = "\nEnter Course Details.\n\n";
                        send(client_socket, add_new_courses, sizeof(add_new_courses), 0);

                        char en_course_id[30] = "Enter Course ID: ";
                        send(client_socket, en_course_id, sizeof(en_course_id), 0);
                        char new_course_id[20];
                        recv(client_socket, new_course_id, sizeof(new_course_id), 0);

                        char en_course_name[30] = "Enter Course Name: ";
                        send(client_socket, en_course_name, sizeof(en_course_name), 0);
                        char new_course_name[30];
                        recv(client_socket, new_course_name, sizeof(new_course_name), 0);

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
                            remove_course_details_course_stu_using_course_id(take_course_id, "data/courses_data/course_and_students.txt");
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
                            char update_status[40] = "\nDetails Updated Successfully";
                            send(client_socket, update_status, sizeof(update_status), 0);
                        }
                        else
                        {
                            char update_status[40] = "\nUpdation Failed";
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
                            strcpy(pass_update_status, "\nPassword Changed Successfully");
                        }
                        else
                        {
                            strcpy(pass_update_status, "\nCheck Your Current Password");
                        }
                        send(client_socket, pass_update_status, sizeof(pass_update_status), 0);
                    }
                    // Logout and Exit
                    else if (choice == 6)
                    {
                        char bye_bye[40] = "\nSigning Out.....Please Wait..\n";
                        send(client_socket, bye_bye, sizeof(bye_bye), 0);
                        char bye[20] = "\nBye Bye....\n";
                        send(client_socket, bye, sizeof(bye), 0);
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
                strcpy(auth_succ_fail, "\nLog in Successful");
                send(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
                char admin_menu[250] = "\nWhat do you want to do?\n\n1) Add student\n2) View Student Details\n3) Add Faculty\n4) View Faculty Details\n5) Activate Student\n6) Block Student\n7) Modify Student Details\n8) Modify Faculty Details\n9) Exit\n\nEnter Your Choice: ";
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
                        char succ[40] = "\nStudent record added successfully\n";
                        send(client_socket, succ, sizeof(succ), 0);
                    }
                    // View Student Details
                    else if (choice == 2)
                    {
                        char give_stu_id[100] = "\nEnter Student ID: ";
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
                        char succ[40] = "\nFaculty record added successfully\n";
                        send(client_socket, succ, sizeof(succ), 0);
                    }
                    // View Faculty Details
                    else if (choice == 4)
                    {
                        char give_fac_id[100] = "\nEnter Faculty ID: ";
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
                        char en_stu_id[25] = "\nEnter Student ID: ";
                        send(client_socket, en_stu_id, sizeof(en_stu_id), 0);
                        char activate_stu_id[50];
                        recv(client_socket, activate_stu_id, sizeof(activate_stu_id), 0);
                        char activation_flag[40];
                        char temp_buff[5] = "1";
                        char temp_buff_status[20] = "account status";
                        char filename[] = "data/students_data/student_data.txt";
                        if (update_student_details(activate_stu_id, temp_buff_status, temp_buff, filename))
                        {
                            strcpy(activation_flag, "\nStudent account Activated Successfully");
                        }
                        else
                        {
                            strcpy(activation_flag, "\nStudent ID not found");
                        }
                        send(client_socket, activation_flag, sizeof(activation_flag), 0);
                    }
                    // Block Student
                    else if (choice == 6)
                    {
                        char en_stu_id[25] = "\nEnter Student ID: ";
                        send(client_socket, en_stu_id, sizeof(en_stu_id), 0);
                        char block_stu_id[50];
                        recv(client_socket, block_stu_id, sizeof(block_stu_id), 0);
                        char activation_flag[40];
                        char temp_buff[5] = "0";
                        if (update_student_details(block_stu_id, "account status", temp_buff, "data/students_data/student_data.txt"))
                        {
                            strcpy(activation_flag, "\nStudent account Blocked Successfully");
                        }
                        else
                        {
                            strcpy(activation_flag, "\nStudent ID not found");
                        }
                        send(client_socket, activation_flag, sizeof(activation_flag), 0);
                    }
                    // Modify Student Details
                    else if (choice == 7)
                    {
                        char en_login[45] = "\nEnter Log in ID of the Student: ";
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
                            char update_status[40] = "\nDetails Updated Successfully";
                            send(client_socket, update_status, sizeof(update_status), 0);
                        }
                        else
                        {
                            char update_status[40] = "\nUpdation Failed";
                            send(client_socket, update_status, sizeof(update_status), 0);
                        }
                    }
                    // Moify Faculty Details
                    else if (choice == 8)
                    {
                        char en_login[45] = "\nEnter Log in ID of the Faculty: ";
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
                            char update_status[40] = "\nDetails Updated Successfully";
                            send(client_socket, update_status, sizeof(update_status), 0);
                        }
                        else
                        {
                            char update_status[40] = "\nUpdation Failed";
                            send(client_socket, update_status, sizeof(update_status), 0);
                        }
                    }
                    // LogOut and Exit
                    else if (choice == 9)
                    {
                        char bye_bye[40] = "\nSigning Out.....Please Wait..\n";
                        send(client_socket, bye_bye, sizeof(bye_bye), 0);
                        char bye[20] = "\nBye Bye....\n";
                        send(client_socket, bye, sizeof(bye), 0);
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
//New Client part end

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
        printf("%d\n", clients_count);
        addr_size = sizeof(new_addr);
        new_socket = accept(server_socket, (struct sockaddr *)&new_addr, &addr_size);
        if (clients_count >= MAX_CLIENTS)
        {
            printf("Maximum number of clients reached. Connection rejected.\n");
            close(new_socket);
        }
        clients_count++;
        if (pthread_create(&tid[clients_count], NULL, handle_client, &new_socket) != 0)
        {
            perror("Error in creating thread");
        }
    }
    close(server_socket);
    // DO NOT TOUCH CODE END
}