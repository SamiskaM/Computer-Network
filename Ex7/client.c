
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define PORT 8080
#define SIZE 1024
#define MAX_RETRY 3

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server;

    char request[SIZE];
    char response[SIZE];

    socklen_t len = sizeof(server);

    if (argc != 2)
    {
        printf("Usage: %s <server-IP>\n", argv[0]);
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Socket error");
        return 1;
    }

    /* 2 second timeout */
    struct timeval timeout;

    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    setsockopt(sockfd,
               SOL_SOCKET,
               SO_RCVTIMEO,
               &timeout,
               sizeof(timeout));

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET,
                  argv[1],
                  &server.sin_addr) <= 0)
    {
        printf("Invalid IP address.\n");
        close(sockfd);
        return 1;
    }

    while (1)
    {
        int choice;

        printf("\n====================================\n");
        printf("       LIBRARY MANAGEMENT\n");
        printf("====================================\n");
        printf("1. View Available Books\n");
        printf("2. Search Book\n");
        printf("3. Issue Book\n");
        printf("4. Return Book\n");
        printf("5. Exit\n");
        printf("====================================\n");
        printf("Enter choice : ");

        scanf("%d", &choice);

        if (choice == 5)
            break;

        if (choice == 1)
        {
            strcpy(request, "LIST");
        }
        else if (choice >= 2 && choice <= 4)
        {
            int id;

            printf("Enter Book ID : ");
            scanf("%d", &id);

            if (choice == 2)
                sprintf(request, "SEARCH %d", id);

            else if (choice == 3)
                sprintf(request, "ISSUE %d", id);

            else
                sprintf(request, "RETURN %d", id);
        }
        else
        {
            printf("Invalid choice.\n");
            continue;
        }

        int success = 0;

        /* Send request with retransmission */
        for (int attempt = 1; attempt <= MAX_RETRY; attempt++)
        {
            sendto(sockfd,
                   request,
                   strlen(request),
                   0,
                   (struct sockaddr *)&server,
                   len);

            printf("\nRequest sent. Attempt %d\n", attempt);

            int n = recvfrom(sockfd,
                             response,
                             SIZE - 1,
                             0,
                             (struct sockaddr *)&server,
                             &len);

            if (n >= 0)
            {
                response[n] = '\0';

                printf("\n------------- SERVER RESPONSE -------------\n");
                printf("%s\n", response);
                printf("-------------------------------------------\n");

                success = 1;
                break;
            }

            printf("Timeout! Retransmitting...\n");
        }

        if (!success)
            printf("Server not responding.\n");
    }

    close(sockfd);

    printf("\nClient terminated.\n");

    return 0;
}
