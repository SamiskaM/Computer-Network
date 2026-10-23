
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SIZE 1024

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    char buffer[SIZE];
    char response[SIZE];

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

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET,
                  argv[1],
                  &server.sin_addr) != 1)
    {
        printf("Invalid IP address\n");
        close(sockfd);
        return 1;
    }

    while (1)
    {
        double a, b;
        char op;

        printf("\n====================================\n");
        printf("          UDP CALCULATOR\n");
        printf("====================================\n");
        printf("Enter expression (Example: 10 + 5)\n");
        printf("Enter 0 to exit\n");
        printf("Expression : ");

        if (fgets(buffer, SIZE, stdin) == NULL)
            break;

        if (strcmp(buffer, "0\n") == 0 ||
            strcmp(buffer, "0") == 0)
            break;

        if (sscanf(buffer, "%lf %c %lf",
                   &a, &op, &b) != 3)
        {
            printf("Invalid input.\n");
            continue;
        }

        if (op != '+' && op != '-' &&
            op != '*' && op != '/')
        {
            printf("Invalid operator.\n");
            continue;
        }

        if (op == '/' && b == 0)
        {
            printf("Division by zero is not allowed.\n");
            continue;
        }

        sprintf(buffer,
                "%.2f %c %.2f",
                a, op, b);

        sendto(sockfd,
               buffer,
               strlen(buffer),
               0,
               (struct sockaddr *)&server,
               len);

        int n = recvfrom(sockfd,
                         response,
                         SIZE - 1,
                         0,
                         (struct sockaddr *)&server,
                         &len);

        if (n < 0)
        {
            perror("Receive error");
            continue;
        }

        response[n] = '\0';

        printf("\nServer Response : %s\n",
               response);
    }

    close(sockfd);

    printf("\nClient terminated.\n");

    return 0;
}

