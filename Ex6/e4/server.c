
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define MAX 100

struct Student
{
    long long regno;
    char name[50];
    char dept[30];
    int semester;
    float marks;
    float cgpa;
};

struct Student students[MAX];
int count = 0;

void loadData()
{
    FILE *fp = fopen("students.txt", "r");

    if (fp == NULL)
        return;

    count = 0;

    while (count < MAX &&
           fscanf(fp, "%lld %49s %29s %d %f %f",
                  &students[count].regno,
                  students[count].name,
                  students[count].dept,
                  &students[count].semester,
                  &students[count].marks,
                  &students[count].cgpa) == 6)
    {
        count++;
    }

    fclose(fp);
}

void saveData()
{
    FILE *fp = fopen("students.txt", "w");

    if (fp == NULL)
        return;

    for (int i = 0; i < count; i++)
    {
        fprintf(fp, "%lld %s %s %d %.2f %.2f\n",
                students[i].regno,
                students[i].name,
                students[i].dept,
                students[i].semester,
                students[i].marks,
                students[i].cgpa);
    }

    fclose(fp);
}

int findStudent(long long regno)
{
    for (int i = 0; i < count; i++)
    {
        if (students[i].regno == regno)
            return i;
    }

    return -1;
}

void processRequest(char request[], char response[])
{
    int choice;
    long long regno;
    struct Student st;

    sscanf(request, "%d", &choice);

    /* ADD */
    if (choice == 1)
    {
        sscanf(request, "%d %lld %49s %29s %d %f %f",
               &choice,
               &st.regno,
               st.name,
               st.dept,
               &st.semester,
               &st.marks,
               &st.cgpa);

        if (count >= MAX)
        {
            strcpy(response, "Database is full.");
            return;
        }

        if (findStudent(st.regno) != -1)
        {
            strcpy(response, "Student already exists.");
            return;
        }

        students[count] = st;
        count++;

        saveData();

        sprintf(response,
                "Student added successfully.\n"
                "Register Number : %lld\n"
                "Name            : %s\n"
                "Department      : %s\n"
                "Semester        : %d\n"
                "Marks           : %.2f\n"
                "CGPA            : %.2f",
                st.regno,
                st.name,
                st.dept,
                st.semester,
                st.marks,
                st.cgpa);
    }

    /* SEARCH */
    else if (choice == 2)
    {
        sscanf(request, "%d %lld", &choice, &regno);

        int pos = findStudent(regno);

        if (pos == -1)
        {
            strcpy(response, "Student not found.");
            return;
        }

        st = students[pos];

        sprintf(response,
                "Student Details\n"
                "-------------------------\n"
                "Register Number : %lld\n"
                "Name            : %s\n"
                "Department      : %s\n"
                "Semester        : %d\n"
                "Marks           : %.2f\n"
                "CGPA            : %.2f",
                st.regno,
                st.name,
                st.dept,
                st.semester,
                st.marks,
                st.cgpa);
    }

    /* UPDATE */
    else if (choice == 3)
    {
        sscanf(request, "%d %lld %49s %29s %d %f %f",
               &choice,
               &st.regno,
               st.name,
               st.dept,
               &st.semester,
               &st.marks,
               &st.cgpa);

        int pos = findStudent(st.regno);

        if (pos == -1)
        {
            strcpy(response, "Student not found.");
            return;
        }

        students[pos] = st;

        saveData();

        strcpy(response, "Student updated successfully.");
    }

    /* DELETE */
    else if (choice == 4)
    {
        sscanf(request, "%d %lld", &choice, &regno);

        int pos = findStudent(regno);

        if (pos == -1)
        {
            strcpy(response, "Student not found.");
            return;
        }

        for (int i = pos; i < count - 1; i++)
        {
            students[i] = students[i + 1];
        }

        count--;

        saveData();

        strcpy(response, "Student deleted successfully.");
    }

    /* DISPLAY */
    else if (choice == 5)
    {
        if (count == 0)
        {
            strcpy(response, "No student records found.");
            return;
        }

        response[0] = '\0';

        for (int i = 0; i < count; i++)
        {
            char temp[300];

            sprintf(temp,
                    "Register Number : %lld\n"
                    "Name            : %s\n"
                    "Department      : %s\n"
                    "Semester        : %d\n"
                    "Marks           : %.2f\n"
                    "CGPA            : %.2f\n"
                    "-------------------------\n",
                    students[i].regno,
                    students[i].name,
                    students[i].dept,
                    students[i].semester,
                    students[i].marks,
                    students[i].cgpa);

            strcat(response, temp);
        }
    }

    else
    {
        strcpy(response, "Invalid choice.");
    }
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    char request[1024];
    char response[4096];

    loadData();

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket error");
        return 1;
    }

    int option = 1;

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
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("Listen error");
        close(server_fd);
        return 1;
    }

    printf("\n====================================\n");
    printf("      STUDENT RESULT SERVER\n");
    printf("====================================\n");
    printf("Port : %d\n", PORT);
    printf("Server waiting for client...\n");

    while (1)
    {
        client_fd = accept(server_fd,
                           (struct sockaddr *)&client,
                           &client_len);

        if (client_fd < 0)
        {
            perror("Accept error");
            continue;
        }

        printf("\nClient connected.\n");

        while (1)
        {
            memset(request, 0, sizeof(request));

            int n = recv(client_fd,
                         request,
                         sizeof(request) - 1,
                         0);

            if (n <= 0)
                break;

            request[n] = '\0';

            if (strcmp(request, "EXIT") == 0)
                break;

            memset(response, 0, sizeof(response));

            processRequest(request, response);

            send(client_fd,
                 response,
                 strlen(response),
                 0);
        }

        close(client_fd);

        printf("Client disconnected.\n");
    }

    close(server_fd);

    return 0;
}
