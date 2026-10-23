
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SIZE 1024

int main()
{
    int sockfd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    char buffer[SIZE];
    char result[SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Socket error");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(sockfd,
             (struct sockaddr *)&server,
             sizeof(server)) < 0)
    {
        perror("Bind error");
        close(sockfd);
        return 1;
    }

    printf("\n====================================\n");
    printf("       UDP CALCULATOR SERVER\n");
    printf("====================================\n");
    printf("Port : %d\n", PORT);
    printf("Server is ready...\n");

    while (1)
    {
        memset(buffer, 0, SIZE);
        memset(result, 0, SIZE);

        int n = recvfrom(sockfd,
                         buffer,
                         SIZE - 1,
                         0,
                         (struct sockaddr *)&client,
                         &len);

        if (n < 0)
            continue;

        buffer[n] = '\0';

        double a, b, answer;
        char op;

        if (sscanf(buffer, "%lf %c %lf",
                   &a, &op, &b) != 3)
        {
            strcpy(result, "Invalid input");
        }
        else
        {
            switch (op)
            {
                case '+':
                    answer = a + b;
                    sprintf(result, "Result = %.2f", answer);
                    break;

                case '-':
                    answer = a - b;
                    sprintf(result, "Result = %.2f", answer);
                    break;

                case '*':
                    answer = a * b;
                    sprintf(result, "Result = %.2f", answer);
                    break;

                case '/':
                    if (b == 0)
                    {
                        strcpy(result,
                               "Error: Division by zero");
                    }
                    else
                    {
                        answer = a / b;
                        sprintf(result,
                                "Result = %.2f",
                                answer);
                    }
                    break;

                default:
                    strcpy(result,
                           "Error: Invalid operator");
            }
        }

        printf("\nRequest  : %s\n", buffer);
        printf("Response : %s\n", result);

        sendto(sockfd,
               result,
               strlen(result) + 1,
               0,
               (struct sockaddr *)&client,
               len);
    }

    close(sockfd);

    return 0;
}

