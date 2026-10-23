
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081

int main()
{
    int sock;
    struct sockaddr_in server;

    char request[1024];
    char response[4096];

    int choice;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket error");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock,
                (struct sockaddr *)&server,
                sizeof(server)) < 0)
    {
        perror("Connection error");
        close(sock);
        return 1;
    }

    while (1)
    {
        printf("\n====================================\n");
        printf("       STUDENT RESULT SYSTEM\n");
        printf("====================================\n");
        printf("1. Add Student\n");
        printf("2. Search Student\n");
        printf("3. Update Student\n");
        printf("4. Delete Student\n");
        printf("5. Display Students\n");
        printf("0. Exit\n");
        printf("====================================\n");

        printf("Enter choice : ");
        scanf("%d", &choice);

        if (choice == 0)
        {
            send(sock, "EXIT", 4, 0);
            break;
        }

        long long regno;
        char name[50];
        char dept[30];
        int semester;
        float marks;
        float cgpa;

        if (choice == 1 || choice == 3)
        {
            printf("Register Number : ");
            scanf("%lld", &regno);

            printf("Name            : ");
            scanf("%49s", name);

            printf("Department      : ");
            scanf("%29s", dept);

            printf("Semester        : ");
            scanf("%d", &semester);

            printf("Marks           : ");
            scanf("%f", &marks);

            printf("CGPA            : ");
            scanf("%f", &cgpa);

            sprintf(request,
                    "%d %lld %s %s %d %.2f %.2f",
                    choice,
                    regno,
                    name,
                    dept,
                    semester,
                    marks,
                    cgpa);
        }

        else if (choice == 2 || choice == 4)
        {
            printf("Register Number : ");
            scanf("%lld", &regno);

            sprintf(request,
                    "%d %lld",
                    choice,
                    regno);
        }

        else if (choice == 5)
        {
            sprintf(request, "%d", choice);
        }

        else
        {
            printf("Invalid choice.\n");
            continue;
        }

        send(sock,
             request,
             strlen(request),
             0);

        memset(response, 0, sizeof(response));

        int n = recv(sock,
                     response,
                     sizeof(response) - 1,
                     0);

        if (n <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }

        response[n] = '\0';

        printf("\n------------- SERVER RESPONSE -------------\n");
        printf("%s\n", response);
        printf("--------------------------------------------\n");
    }

    close(sock);

    return 0;
}
