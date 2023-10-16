# Software System Mini Project: Design and Development of Course Registration Porta
- The "Design and Development of Course Registration Portal (Academia)" is an ambitious project aimed at creating a comprehensive academic portal that addresses the evolving needs of educational institutions and students. This portal will serve as a user-friendly and multifunctional platform, revolutionizing the way course registration and academic management are conducted.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [FAQ](#faq)
- [RoadMap](#roadmap)
- [License](#license)

## Prerequisites:
- Before you can install and run the Academia Portal, you need to have a C programming environment installed on your system. This typically includes a C compiler, such as GCC (GNU Compiler Collection).
- C Compiler: You can download and install the GCC compiler from the official website or use your system's package manager.
- Please make sure that the C compiler is successfully installed before proceeding with the installation of the Academia Portal.


## Installation
- To get your Academia Portal up and running, follow these installation steps:

Step 1: Clone the repository:   
    ```
        git clone https://github.com/rishabh0014/SS_mini_Project.git
        cd SS_mini_Project
    ```
Step 2: Compile and Run the server program
    - Compile
    ```
        gcc -o server server.c
    ```
    - Run
    ```
        ./server
    ```

Step 2: Compile and Run the client program
    - Compile
    ```
        gcc -o client client.c
    ```
    - Run
    ```
        ./client
    ```

Step 3: Initial Configuration
- To start with the Academia Portal, you can use the following initial IDs and passwords for the admin, faculty, and student accounts. After your first login, it's essential to change your password for security reasons. (You can not change the Admin password for now)

- Admin Account:  
    - Login ID: `admin`
    - Password: `1234`
- Faculty Account: 
    - Login ID: `faculty`
    - Password: `1234`
- Student Account:  
    - Login ID: `student`
    - Password: `1234`
- Please ensure that you change your password after the initial login to secure your account.

## FAQ
- I will update this section at the end :)

## Roadmap

- Here's what we have planned for the future of the Academia Portal:
  - Enhanced User Profiles: Improve user profile management with additional features and customization options.
  - Integrated Messaging System: Implement a built-in messaging system for better communication within the portal.
  - Advanced Course Management: Expand course management features, including course scheduling, resource allocation, and course analytics.
  - For enhanced security, it's recommended that you implement a password reset or account creation process that allows users to set their own passwords during their first login.

## License
This project is licensed under the XYZ License - see the [LICENSE](dummy) file for details.
