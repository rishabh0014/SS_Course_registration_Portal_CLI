#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h> 

#define PORT 8080
#define MAX_CLIENTS 10

struct User {
    char username[50];
    char password[50];
    int num;
};

int readUsersFromFile(struct User users[], const char* filename) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("Error opening file");
        return 0;
    }

    int count = 0;
    char buffer[1024];
    ssize_t bytesRead;

    while ((bytesRead = read(file_fd, buffer, sizeof(buffer))) > 0) {
        char *token = strtok(buffer, " \n");
        while (token != NULL) {
            strcpy(users[count].username, token);
            token = strtok(NULL, " \n");
            if (token != NULL) {
                strcpy(users[count].password, token);
                token = strtok(NULL, " \n");
                count++;
            }
        }
    }

    close(file_fd);
    return count;
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char username[50], password[50];
    int num;
    int auth_status = 0;

    read(client_socket, username, sizeof(username));
    read(client_socket, password, sizeof(password));
    read(client_socket, &num, sizeof(int));
    
    struct User users[MAX_CLIENTS];

    int users_count;
    if(num==1){
        users_count = readUsersFromFile(users, "data/students_data/student_log_in.txt");
    }
    else if(num==2){
        users_count = readUsersFromFile(users, "data/faculties_data/faculties_log_in.txt");
    }
    else if(num==3){
        users_count = readUsersFromFile(users, "data/admins_data/admin_log_in.txt");
    }

    for (int i = 0; i < users_count; i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            auth_status = 1; 
            break;
        }
    }

    send(client_socket, &auth_status, sizeof(int), 0);

    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    pthread_t tid[MAX_CLIENTS];
    int clients_count = 0;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error in socket creation");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in binding");
        exit(1);
    }

    if (listen(server_socket, MAX_CLIENTS) == 0) {
        printf("Listening...\n");
    } else {
        perror("Error in listening");
        exit(1);
    }

    while (1) {
        addr_size = sizeof(new_addr);
        new_socket = accept(server_socket, (struct sockaddr*)&new_addr, &addr_size); 

        if (pthread_create(&tid[clients_count], NULL, handle_client, &new_socket) != 0) {
            perror("Error in creating thread");
        }

        clients_count++;

        if (clients_count >= MAX_CLIENTS) {
            printf("Maximum number of clients reached. Connection rejected.\n");
            close(new_socket);
        }
    }

    close(server_socket);

    return 0;
}
