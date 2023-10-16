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
#define MAX_COURSES 50

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

void remove_new_line(char *str)
{
    if (str != NULL && *str == '\n')
    {
        while (*str != '\0')
        {
            *str = *(str + 1);
            str++;
        }
    }
}

int main()
{
    int client_socket, num;
    struct sockaddr_in server_addr;
    int auth_status, change_pass_status;
    char username[50], password[50], curr_password[50], new_password[50], welcome[100], select_role[100], en_username[20], en_password[20], auth_succ_fail[40];

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
                recv(client_socket,auth_succ_fail,sizeof(auth_succ_fail),0);
                printf("%s\n",auth_succ_fail);
                printf("Hello, %s! Welcome..\n", username);
                char student_menu[200];
                while (1)
                {
                    recv(client_socket, student_menu, sizeof(student_menu), 0);
                    printf("%s", student_menu);
                    int choice;

                    scanf("%d", &choice);
                    send(client_socket, &choice, sizeof(int), 0);

                    // View All Courses
                    if (choice == 1)
                    {
                        int ack = 1;
                        send(client_socket, &ack, sizeof(int), 0);
                        if (ack)
                        {
                            char list_of_all[30];
                            recv(client_socket, list_of_all, sizeof(list_of_all), 0);
                            printf("%s\n\n", list_of_all);
                            int total_course;
                            recv(client_socket, &total_course, sizeof(int), 0);

                            struct Course course_detail;
                            for (int i = 0; i < total_course-1; i++)
                            {
                                recv(client_socket, &course_detail, sizeof(course_detail), 0);
                                remove_new_line(course_detail.course_id);
                                remove_new_line(course_detail.course_name);
                                remove_new_line(course_detail.faculty_id);
                                remove_new_line(course_detail.max_seats);
                                remove_new_line(course_detail.rem_seats);
                                printf("Course ID: %s\n", course_detail.course_id);
                                printf("Course Name: %s\n", course_detail.course_name);
                                printf("Faculty ID: %s\n", course_detail.faculty_id);
                                printf("Seats: %s\n", course_detail.max_seats);
                                printf("Remaining Seats: %s\n", course_detail.rem_seats);
                                printf("\n");
                            }
                        }
                    }
                    // Enroll new Cources
                    else if (choice == 2)
                    {
                        char en_new_course[30];
                        recv(client_socket,en_new_course,sizeof(en_new_course),0);
                        printf("%s",en_new_course);
                        char course_id[20];
                        getchar();
                        scanf("%[^\n]",course_id);
                        send(client_socket,course_id,sizeof(course_id),0);
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
                        char en_old[30];
                        recv(client_socket, en_old, sizeof(en_old), 0);
                        printf("%s", en_old);
                        char old_password[50];
                        getchar();
                        scanf("%[^\n]", old_password);
                        send(client_socket, old_password, sizeof(old_password), 0);

                        char en_new[30];
                        recv(client_socket, en_new, sizeof(en_new), 0);
                        printf("%s", en_new);
                        char new_password[50];
                        scanf("%s", new_password);
                        send(client_socket, new_password, sizeof(new_password), 0);

                        char pass_update_status[50];
                        recv(client_socket, pass_update_status, sizeof(pass_update_status), 0);
                        printf("%s\n", pass_update_status);
                    }
                    // LogOut and Exit
                    else if (choice == 6)
                    {
                        exit(0);
                    }
                }
            }
            // Authentication failed Student
            else{
                recv(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
                printf("%s\n",auth_succ_fail);
            }
        }
        // faculty
        else if (num == 2)
        {
            recv(client_socket, &auth_status, sizeof(int), 0);
            if (auth_status == 1)
            {
                recv(client_socket,auth_succ_fail,sizeof(auth_succ_fail),0);
                printf("%s\n",auth_succ_fail);
                printf("Hello, %s! Welcome..\n", username);
                char faculty_menu[200];
                while (1)
                {
                    recv(client_socket, faculty_menu, sizeof(faculty_menu), 0);
                    printf("%s", faculty_menu);
                    int choice;
                    scanf("%d", &choice);
                    send(client_socket, &choice, sizeof(int), 0);

                    // View Offered Courses
                    if (choice == 1)
                    {
                        int ack = 1;
                        send(client_socket, &ack, sizeof(int), 0);
                        if (ack)
                        {
                            char list_of_all[30];
                            recv(client_socket, list_of_all, sizeof(list_of_all), 0);
                            printf("%s\n\n", list_of_all);
                            int total_course;
                            recv(client_socket, &total_course, sizeof(int), 0);

                            struct Course course_detail;
                            for (int i = 0; i < total_course - 1; i++)
                            {
                                recv(client_socket, &course_detail, sizeof(course_detail), 0);
                                remove_new_line(course_detail.course_id);
                                remove_new_line(course_detail.course_name);
                                remove_new_line(course_detail.faculty_id);
                                remove_new_line(course_detail.max_seats);
                                remove_new_line(course_detail.rem_seats);
                                if (sizeof(course_detail.course_id) - 1 == 0)
                                    continue;
                                printf("Course ID: %s\n", course_detail.course_id);
                                printf("Course Name: %s\n", course_detail.course_name);
                                printf("Faculty ID: %s\n", course_detail.faculty_id);
                                printf("Seats: %s\n", course_detail.max_seats);
                                printf("Remaining Seats: %s\n", course_detail.rem_seats);
                                printf("\n");
                            }
                        }
                    }
                    // Add new Cources
                    else if (choice == 2)
                    {
                        char add_new_courses[30];
                        recv(client_socket, add_new_courses, sizeof(add_new_courses), 0);
                        printf("%s", add_new_courses);

                        char en_course_id[30];
                        recv(client_socket, en_course_id, sizeof(en_course_id), 0);
                        printf("%s", en_course_id);
                        char new_course_id[20];
                        getchar();
                        scanf("%[^\n]", new_course_id);
                        send(client_socket, new_course_id, sizeof(new_course_id), 0);

                        char en_course_name[30];
                        recv(client_socket, en_course_name, sizeof(en_course_name), 0);
                        printf("%s", en_course_name);
                        char new_course_name[30];
                        getchar();
                        scanf("%[^\n]", new_course_name);
                        send(client_socket, new_course_name, sizeof(new_course_name), 0);

                        char en_number_seats[30];
                        recv(client_socket, en_number_seats, sizeof(en_number_seats), 0);
                        printf("%s", en_number_seats);
                        char new_number_seats[20];
                        getchar();
                        scanf("%[^\n]", new_number_seats);
                        send(client_socket, new_number_seats, sizeof(new_number_seats), 0);

                        char added_succ[30];
                        recv(client_socket, added_succ, sizeof(added_succ), 0);
                        printf("%s\n", added_succ);
                    }
                    // Remove Courses from Catalog
                    else if (choice == 3)
                    {
                        char en_course_id[30];
                        recv(client_socket, en_course_id, sizeof(en_course_id), 0);
                        printf("%s\n", en_course_id);
                        char take_course_id[20];
                        getchar();
                        scanf("%[^\n]", take_course_id);
                        send(client_socket, take_course_id, sizeof(take_course_id), 0);
                        char remove_status[30];
                        recv(client_socket, remove_status, sizeof(remove_status), 0);
                        printf("%s\n", remove_status);
                    }
                    // Update Course Details
                    else if (choice == 4)
                    {
                        char en_course_id[45];
                        recv(client_socket, en_course_id, sizeof(en_course_id), 0);
                        printf("%s", en_course_id);
                        char course_id[30];
                        getchar();
                        scanf("%[^\n]", course_id);
                        send(client_socket, course_id, sizeof(course_id), 0);

                        char which_det[50];
                        recv(client_socket, which_det, sizeof(which_det), 0);
                        printf("%s", which_det);
                        char this_detail[15];
                        getchar();
                        scanf("%[^\n]", this_detail);
                        send(client_socket, this_detail, sizeof(this_detail), 0);

                        char en_nw_det[20];
                        recv(client_socket, en_nw_det, sizeof(en_nw_det), 0);
                        printf("%s", en_nw_det);
                        char new_data[120];
                        getchar();
                        scanf("%[^\n]", new_data);
                        send(client_socket, new_data, sizeof(new_data), 0);
                        char update_status[40];
                        recv(client_socket, update_status, sizeof(update_status), 0);
                        printf("%s\n", update_status);
                    }
                    // Change Password
                    else if (choice == 5)
                    {
                        char en_old[30];
                        recv(client_socket, en_old, sizeof(en_old), 0);
                        printf("%s", en_old);
                        char old_password[50];
                        getchar();
                        scanf("%[^\n]", old_password);
                        send(client_socket, old_password, sizeof(old_password), 0);

                        char en_new[30];
                        recv(client_socket, en_new, sizeof(en_new), 0);
                        printf("%s", en_new);
                        char new_password[50];
                        scanf("%s", new_password);
                        send(client_socket, new_password, sizeof(new_password), 0);

                        char pass_update_status[50];
                        recv(client_socket, pass_update_status, sizeof(pass_update_status), 0);
                        printf("%s\n", pass_update_status);
                    }
                    // LogOut and Exit
                    else if (choice == 6)
                    {
                        exit(0);
                    }
                }
            }
            // Authentication failed Faculty
            else
            {
                recv(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
                printf("%s\n",auth_succ_fail);
            }
        }
        // admin
        else if (num == 3)
        {
            recv(client_socket, &auth_status, sizeof(int), 0);
            if (auth_status == 1)
            {
                recv(client_socket,auth_succ_fail,sizeof(auth_succ_fail),0);
                printf("%s\n",auth_succ_fail);

                printf("Hello, %s! Welcome..\n", username);
                char admin_menu[200];
                while (1)
                {
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
                            scanf("%[^\n]", student_info.age);
                            getchar();
                            printf("Enter email ID: ");
                            scanf("%[^\n]", student_info.email_id);
                            getchar();
                            printf("Enter Address: ");
                            scanf("%[^\n]", student_info.address);
                            getchar();
                            strcpy(student_info.activate_stu, "1");
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
                            printf("Student Roll No/Log in ID: %s\n", student_info.login_id);
                            printf("Student Password: %s\n", student_info.password);
                            printf("Student Name: %s\n", student_info.name);
                            printf("Student age: %s\n", student_info.age);
                            printf("Student Email ID: %s\n", student_info.email_id);
                            printf("Student Address: %s\n", student_info.address);
                            printf("Account Status: %s\n", student_info.activate_stu);
                        }
                        else
                        {
                            char not_found[30];
                            recv(client_socket, not_found, sizeof(not_found), 0);
                            printf("%s\n", not_found);
                        }
                    }
                    // Add Faculty
                    else if (choice == 3)
                    {
                        struct Faculty faculty_info;
                        // Taking faculty Info
                        {
                            getchar();
                            printf("Enter Log in ID: ");
                            scanf("%[^\n]", faculty_info.login_id);
                            getchar();
                            printf("Enter Password: ");
                            scanf("%[^\n]", faculty_info.password);
                            getchar();
                            printf("Enter Name: ");
                            scanf("%[^\n]", faculty_info.name);
                            getchar();
                            printf("Enter Department: ");
                            scanf("%s", faculty_info.department);
                            getchar();
                            printf("Enter Designation: ");
                            scanf("%s", faculty_info.designation);
                            getchar();
                            printf("Enter email ID: ");
                            scanf("%[^\n]", faculty_info.email_id);
                            getchar();
                            printf("Enter Address: ");
                            scanf("%[^\n]", faculty_info.address);
                            getchar();
                            char buffer[sizeof(struct Faculty)];
                            memcpy(buffer, &faculty_info, sizeof(struct Faculty));
                            send(client_socket, buffer, sizeof(struct Faculty), 0);
                        }
                    }
                    // View Faculty Details
                    else if (choice == 4)
                    {
                        char give_fac_id[100];
                        recv(client_socket, give_fac_id, sizeof(give_fac_id), 0);
                        printf("%s", give_fac_id);
                        char find_login_id[50];
                        scanf("%s", find_login_id);
                        send(client_socket, find_login_id, sizeof(find_login_id), 0);
                        int fac_found;
                        recv(client_socket, &fac_found, sizeof(int), 0);
                        if (fac_found)
                        {
                            char buffer[sizeof(struct Faculty)];
                            recv(client_socket, buffer, sizeof(struct Faculty), 0);
                            struct Faculty faculty_info;
                            memcpy(&faculty_info, buffer, sizeof(struct Faculty));
                            printf("Student Details\n");
                            printf("Student Roll No/Log in ID: %s\n", faculty_info.login_id);
                            printf("Student Password: %s\n", faculty_info.password);
                            printf("Student Name: %s\n", faculty_info.name);
                            printf("Student department: %s\n", faculty_info.department);
                            printf("Student designation: %s\n", faculty_info.designation);
                            printf("Student Email ID: %s\n", faculty_info.email_id);
                            printf("Student Address: %s\n", faculty_info.address);
                        }
                        else
                        {
                            char not_found[30];
                            recv(client_socket, not_found, sizeof(not_found), 0);
                            printf("%s\n", not_found);
                        }
                    }
                    // Activate Student
                    else if (choice == 5)
                    {
                        char en_stu_id[25];
                        recv(client_socket, en_stu_id, sizeof(en_stu_id), 0);
                        printf("%s", en_stu_id);
                        getchar();
                        char activate_stu_id[50];
                        scanf("%[^\n]", activate_stu_id);
                        send(client_socket, activate_stu_id, sizeof(activate_stu_id), 0);
                        char activation_flag[40];
                        recv(client_socket, activation_flag, sizeof(activation_flag), 0);
                        printf("%s\n", activation_flag);
                    }
                    // Block Student
                    else if (choice == 6)
                    {
                        char en_stu_id[25];
                        recv(client_socket, en_stu_id, sizeof(en_stu_id), 0);
                        printf("%s", en_stu_id);
                        getchar();
                        char block_stu_id[50];
                        scanf("%[^\n]", block_stu_id);
                        send(client_socket, block_stu_id, sizeof(block_stu_id), 0);
                        char activation_flag[40];
                        recv(client_socket, activation_flag, sizeof(activation_flag), 0);
                        printf("%s\n", activation_flag);
                    }
                    // Modify Student Details
                    else if (choice == 7)
                    {
                        char en_login[45];
                        recv(client_socket, en_login, sizeof(en_login), 0);
                        printf("%s", en_login);
                        char log_in_id[50];
                        getchar();
                        scanf(" %[^\n]", log_in_id);
                        send(client_socket, log_in_id, sizeof(log_in_id), 0);

                        char which_det[50];
                        recv(client_socket, which_det, sizeof(which_det), 0);
                        printf("%s", which_det);
                        char this_detail[15];
                        getchar();
                        scanf(" %[^\n]", this_detail);
                        send(client_socket, this_detail, sizeof(this_detail), 0);

                        char en_nw_det[20];
                        recv(client_socket, en_nw_det, sizeof(en_nw_det), 0);
                        printf("%s", en_nw_det);
                        char new_data[120];
                        getchar();
                        scanf(" %[^\n]", new_data);
                        send(client_socket, new_data, sizeof(new_data), 0);

                        char update_status[40];
                        recv(client_socket, update_status, sizeof(update_status), 0);
                        printf("%s\n", update_status);
                    }
                    // Moify Faculty Details
                    else if (choice == 8)
                    {
                        char en_login[45];
                        recv(client_socket, en_login, sizeof(en_login), 0);
                        printf("%s", en_login);
                        char log_in_id[50];
                        getchar();
                        scanf(" %[^\n]", log_in_id);
                        send(client_socket, log_in_id, sizeof(log_in_id), 0);

                        char which_det[50];
                        recv(client_socket, which_det, sizeof(which_det), 0);
                        printf("%s", which_det);
                        char this_detail[15];
                        getchar();
                        scanf(" %[^\n]", this_detail);
                        send(client_socket, this_detail, sizeof(this_detail), 0);

                        char en_nw_det[20];
                        recv(client_socket, en_nw_det, sizeof(en_nw_det), 0);
                        printf("%s", en_nw_det);
                        char new_data[120];
                        getchar();
                        scanf(" %[^\n]", new_data);
                        send(client_socket, new_data, sizeof(new_data), 0);

                        char update_status[40];
                        recv(client_socket, update_status, sizeof(update_status), 0);
                        printf("%s\n", update_status);
                    }
                    // LogOut and Exit
                    else if (choice == 9)
                    {
                        exit(0);
                    }
                }
            }
            // Authentication fail Admin
            else
            {
                recv(client_socket, auth_succ_fail, sizeof(auth_succ_fail), 0);
                printf("%s\n",auth_succ_fail);
            }
        }
    }

    return 0;
}