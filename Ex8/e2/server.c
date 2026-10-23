#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define PORT 8082
#define SIZE 1024
#define MAX 100

struct Appointment
{
    int id;
    char patient[30];
    char doctor[30];
    char date[20];
    char time[10];
};

struct Appointment a[MAX];
int count = 0;

/* Load appointments */
void load()
{
    FILE *fp = fopen("appointments.txt", "r");

    if (fp == NULL)
        return;

    while (count < MAX &&
           fscanf(fp, "%d %29s %29s %19s %9s",
                  &a[count].id,
                  a[count].patient,
                  a[count].doctor,
                  a[count].date,
                  a[count].time) == 5)
    {
        count++;
    }

    fclose(fp);
}

/* Save appointments */
void save()
{
    FILE *fp = fopen("appointments.txt", "w");

    if (fp == NULL)
        return;

    for (int i = 0; i < count; i++)
    {
        fprintf(fp, "%d %s %s %s %s\n",
                a[i].id,
                a[i].patient,
                a[i].doctor,
                a[i].date,
                a[i].time);
    }

    fclose(fp);
}

/* Log activity */
void logActivity(char msg[])
{
    FILE *fp = fopen("hospital.log", "a");

    if (fp != NULL)
    {
        time_t now = time(NULL);
        fprintf(fp, "%s %s\n", ctime(&now), msg);
        fclose(fp);
    }
}

/* Process request */
void process(char request[], char response[])
{
    int choice, id;
    char patient[30], doctor[30], date[20], time[10];

    sscanf(request, "%d", &choice);

    /* Register patient */
    if (choice == 1)
    {
        sscanf(request, "%d %29s", &choice, patient);

        sprintf(response,
                "Patient %s registered successfully.",
                patient);

        logActivity(response);
    }

    /* Book appointment */
    else if (choice == 2)
    {
        sscanf(request, "%d %29s %29s %19s %9s",
               &choice,
               patient,
               doctor,
               date,
               time);

        for (int i = 0; i < count; i++)
        {
            if (strcmp(a[i].doctor, doctor) == 0 &&
                strcmp(a[i].date, date) == 0 &&
                strcmp(a[i].time, time) == 0)
            {
                strcpy(response,
                       "Appointment slot is already booked.");
                return;
            }
        }

        if (count >= MAX)
        {
            strcpy(response, "Appointment database is full.");
            return;
        }

        a[count].id = count + 1;

        strcpy(a[count].patient, patient);
        strcpy(a[count].doctor, doctor);
        strcpy(a[count].date, date);
        strcpy(a[count].time, time);

        count++;

        save();

        sprintf(response,
                "Appointment booked successfully.\n"
                "Appointment ID : %d",
                a[count - 1].id);

        logActivity(response);
    }

    /* Check doctor availability */
    else if (choice == 3)
    {
        sscanf(request, "%d %29s %19s",
               &choice,
               doctor,
               date);

        sprintf(response,
                "Available slots for Dr.%s on %s:\n",
                doctor,
                date);

        int found = 0;

        for (int h = 9; h <= 16; h++)
        {
            char slot[10];
            sprintf(slot, "%02d:00", h);

            int booked = 0;

            for (int i = 0; i < count; i++)
            {
                if (strcmp(a[i].doctor, doctor) == 0 &&
                    strcmp(a[i].date, date) == 0 &&
                    strcmp(a[i].time, slot) == 0)
                {
                    booked = 1;
                    break;
                }
            }

            if (!booked)
            {
                strcat(response, slot);
                strcat(response, "\n");
                found = 1;
            }
        }

        if (!found)
            strcat(response, "No slots available.");
    }

    /* View appointment */
    else if (choice == 4)
    {
        sscanf(request, "%d %d", &choice, &id);

        for (int i = 0; i < count; i++)
        {
            if (a[i].id == id)
            {
                sprintf(response,
                        "Appointment Details\n"
                        "-------------------------\n"
                        "ID      : %d\n"
                        "Patient : %s\n"
                        "Doctor  : %s\n"
                        "Date    : %s\n"
                        "Time    : %s",
                        a[i].id,
                        a[i].patient,
                        a[i].doctor,
                        a[i].date,
                        a[i].time);

                return;
            }
        }

        strcpy(response, "Appointment not found.");
    }

    /* Patient history */
    else if (choice == 5)
    {
        sscanf(request, "%d %29s",
               &choice,
               patient);

        response[0] = '\0';

        for (int i = 0; i < count; i++)
        {
            if (strcmp(a[i].patient, patient) == 0)
            {
                char temp[150];

                sprintf(temp,
                        "ID:%d  Doctor:%s  Date:%s  Time:%s\n",
                        a[i].id,
                        a[i].doctor,
                        a[i].date,
                        a[i].time);

                strcat(response, temp);
            }
        }

        if (response[0] == '\0')
            strcpy(response, "No appointment history found.");
    }

    else
    {
        strcpy(response, "Invalid choice.");
    }
}

int main()
{
    int server_fd, client_fd;
    int option = 1;

    struct sockaddr_in server;

    char request[SIZE];
    char response[SIZE];

    load();

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket error");
        return 1;
    }

    setsockopt(server_fd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &option,
               sizeof(option));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_fd,
             (struct sockaddr *)&server,
             sizeof(server)) < 0)
    {
        perror("Bind error");
        return 1;
    }

    listen(server_fd, 5);

    printf("\n====================================\n");
    printf("     HOSPITAL APPOINTMENT SERVER\n");
    printf("====================================\n");
    printf("Port : %d\n", PORT);
    printf("Server is ready...\n");

    while (1)
    {
        /* Iterative server */
        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0)
            continue;

        printf("\nClient connected.\n");

        memset(request, 0, SIZE);
        memset(response, 0, SIZE);

        int n = recv(client_fd,
                     request,
                     SIZE - 1,
                     0);

        if (n > 0)
        {
            request[n] = '\0';

            printf("Request : %s\n", request);

            process(request, response);

            send(client_fd,
                 response,
                 strlen(response),
                 0);

            printf("Response sent.\n");
        }

        close(client_fd);

        printf("Client disconnected.\n");
    }

    close(server_fd);

    return 0;
}
