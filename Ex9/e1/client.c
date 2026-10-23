#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8085
#define SIZE 4096

int main()
{
    int socket_fd;
    struct sockaddr_in server_address;

    char username[30];
    char password[30];
    char buffer[SIZE];

    int choice;
    int i;

    printf("====================================\n");
    printf("     ONLINE EXAMINATION SYSTEM\n");
    printf("====================================\n");

    printf("Username : ");
    scanf("%29s", username);

    printf("Password : ");
    scanf("%29s", password);

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(socket_fd,
                (struct sockaddr *)&server_address,
                sizeof(server_address)) < 0)
    {
        perror("Connection failed");
        close(socket_fd);
        return 1;
    }

    /* Send username */
    send(socket_fd,
         username,
         strlen(username) + 1,
         0);

    /* Wait for server acknowledgement */
    memset(buffer, 0, sizeof(buffer));

    recv(socket_fd,
         buffer,
         sizeof(buffer) - 1,
         0);

    /* Send password */
    send(socket_fd,
         password,
         strlen(password) + 1,
         0);

    /* Receive login result */
    memset(buffer, 0, sizeof(buffer));

    if (recv(socket_fd,
             buffer,
             sizeof(buffer) - 1,
             0) <= 0)
    {
        printf("Server disconnected.\n");
        close(socket_fd);
        return 1;
    }

    if (strcmp(buffer, "LOGIN_SUCCESS") != 0)
    {
        printf("\nLogin failed.\n");
        printf("Invalid username or password.\n");

        close(socket_fd);
        return 0;
    }

    printf("\nLogin successful!\n");

    while (1)
    {
        printf("\n====================================\n");
        printf("        ONLINE EXAMINATION\n");
        printf("====================================\n");
        printf("1. View Questions\n");
        printf("2. Submit Answers\n");
        printf("3. View Final Score\n");
        printf("0. Logout\n");
        printf("====================================\n");

        printf("Enter choice : ");
        scanf("%d", &choice);

        sprintf(buffer, "%d", choice);

        send(socket_fd,
             buffer,
             strlen(buffer) + 1,
             0);

        if (choice == 1)
        {
            printf("\n------------- QUESTIONS -------------\n");

            while (1)
            {
                memset(buffer, 0, sizeof(buffer));

                if (recv(socket_fd,
                         buffer,
                         sizeof(buffer) - 1,
                         0) <= 0)
                {
                    printf("Server disconnected.\n");
                    close(socket_fd);
                    return 0;
                }

                if (strcmp(buffer, "END_QUESTIONS") == 0)
                    break;

                printf("%s", buffer);
            }

            printf("-------------------------------------\n");
        }

        else if (choice == 2)
        {
            printf("\nEnter answers for 5 questions.\n");
            printf("Use A, B, C or D.\n\n");

            for (i = 1; i <= 5; i++)
            {
                char answer[10];

                printf("Answer for Question %d : ", i);
                scanf("%s", answer);

                send(socket_fd,
                     answer,
                     strlen(answer) + 1,
                     0);
            }

            memset(buffer, 0, sizeof(buffer));

            recv(socket_fd,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

            printf("\n------------- RESULT -------------\n");
            printf("%s\n", buffer);
            printf("----------------------------------\n");
        }

        else if (choice == 3)
        {
            memset(buffer, 0, sizeof(buffer));

            recv(socket_fd,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

            printf("\n------------- SCORE -------------\n");
            printf("%s\n", buffer);
            printf("---------------------------------\n");
        }

        else if (choice == 0)
        {
            memset(buffer, 0, sizeof(buffer));

            recv(socket_fd,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

            printf("\n%s\n", buffer);

            break;
        }

        else
        {
            memset(buffer, 0, sizeof(buffer));

            recv(socket_fd,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

            printf("\nServer : %s\n", buffer);
        }
    }

    close(socket_fd);

    printf("Client terminated.\n");

    return 0;
}
