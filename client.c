#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>

#define PORT 8080

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
    int client_socket;
    struct sockaddr_in server_addr;
    int auth_status;

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
    printf("%d\n", num);

    char username[50], password[50];
    printf("Enter username: ");
    scanf("%s", username);
    getchar();
    printf("Enter password: ");
    hide_password(password, sizeof(password));
    send(client_socket, username, sizeof(username), 0);
    send(client_socket, password, sizeof(password), 0);
    send(client_socket, &num, sizeof(int), 0);
    

    recv(client_socket, &auth_status, sizeof(int), 0);

    if (auth_status == 1)
    {
        printf("Authentication successful!\n");
        if (num == 1)
        {
            system("gcc -o student_only/student_main.out student_only/student_main.c");
            system("student_only/student_main.out username");
        }
        else if (num == 2)
        {
            system("gcc -o faculty_only/faculty_main.out faculty_only/faculty_main.c");
            system("faculty_only/faculty_main.out");
        }
        else if (num == 3)
        {
            system("gcc -o admin_only/admin_main.out admin_only/admin_main.c");
            system("admin_only/admin_main.out");
        }
    }
    else
    {
        printf("Authentication failed.\n");
    }

    close(client_socket);
    return 0;
}
