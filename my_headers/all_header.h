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