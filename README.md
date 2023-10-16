# Software System Mini Project: Design and Development of Course Registration Porta
- The "Design and Development of Course Registration Portal (Academia)" is an ambitious project aimed at creating a comprehensive academic portal that addresses the evolving needs of educational institutions and students. This portal will serve as a user-friendly and multifunctional platform, revolutionizing the way course registration and academic management are conducted.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)

## How to execute?

Step 1: Get the server up and running   
- Compile the server program  
    ```
        gcc -o server server.c
    ```
- Run the server program
    ```
        ./server
    ```

Step 2: Run the client program
- Compile the client program
    ```bash
        gcc -o client client.c
    ```
- Run the client program for every user
    ```bash
        ./client
    ```
- Credentials to login as the admin  
    - Login ID : `admin`
    - Password : `1234`
- Credentials to login as the faculty  
    - Login ID : `faculty`
    - Password : `1234`
- Credentials to login as the student  
    - Login ID : `student`
    - Password : `1234`
- There are more faculty and student. You can find the credentials in `data\faculties_data\faculty_log_in.txt` and `data\students_data\student_log_in.txt`


