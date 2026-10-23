
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082
#define SIZE 1024

int main()
{
    while (1)
    {
        int choice;

        printf("\n====================================\n");
        printf("     HOSPITAL APPOINTMENT SYSTEM\n");
        printf("====================================\n");
        printf("1. Register Patient\n");
        printf("2. Book Appointment\n");
        printf("3. Check Doctor Availability\n");
        printf("4. View Appointment\n");
        printf("5. Patient History\n");
        printf("0. Exit\n");
        printf("====================================\n");

        printf("Enter choice : ");
        scanf("%d", &choice);

        if (choice == 0)
            break;

        char request[SIZE];
        char response[SIZE];

        if (choice == 1)
        {
            char patient[30];

            printf("Patient name : ");
            scanf("%29s", patient);

            sprintf(request,
                    "1 %s",
                    patient);
        }

        else if (choice == 2)
        {
            char patient[30];
            char doctor[30];
            char date[20];
            char time[10];

            printf("Patient name : ");
            scanf("%29s", patient);

            printf("Doctor name  : ");
            scanf("%29s", doctor);

            printf("Date (YYYY-MM-DD) : ");
            scanf("%19s", date);

            printf("Time (HH:MM) : ");
            scanf("%9s", time);

            sprintf(request,
                    "2 %s %s %s %s",
                    patient,
                    doctor,
                    date,
                    time);
        }

        else if (choice == 3)
        {
            char doctor[30];
            char date[20];

            printf("Doctor name : ");
            scanf("%29s", doctor);

            printf("Date (YYYY-MM-DD) : ");
            scanf("%19s", date);

            sprintf(request,
                    "3 %s %s",
                    doctor,
                    date);
        }

        else if (choice == 4)
        {
            int id;

            printf("Appointment ID : ");
            scanf("%d", &id);

            sprintf(request,
                    "4 %d",
                    id);
        }

        else if (choice == 5)
        {
            char patient[30];

            printf("Patient name : ");
            scanf("%29s", patient);

            sprintf(request,
                    "5 %s",
                    patient);
        }

        else
        {
            printf("Invalid choice.\n");
            continue;
        }

        int sockfd;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
        {
            perror("Socket error");
            continue;
        }

        struct sockaddr_in server;

        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(sockfd,
                    (struct sockaddr *)&server,
                    sizeof(server)) < 0)
        {
            perror("Connection error");
            close(sockfd);
            continue;
        }

        send(sockfd,
             request,
             strlen(request),
             0);

        int n = recv(sockfd,
                     response,
                     SIZE - 1,
                     0);

        if (n > 0)
        {
            response[n] = '\0';

            printf("\n------------- SERVER RESPONSE -------------\n");
            printf("%s\n", response);
            printf("-------------------------------------------\n");
        }

        close(sockfd);
    }

    printf("\nClient terminated.\n");

    return 0;
}
