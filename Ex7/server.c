
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 8080
#define SIZE 1024
#define MAX_BOOKS 100

struct Book
{
    int id;
    char title[50];
    int available;
};

struct Book books[MAX_BOOKS];
int count = 0;

/* Load books from file */
void loadBooks()
{
    FILE *fp = fopen("library.txt", "r");

    if (fp == NULL)
    {
        books[0].id = 1;
        strcpy(books[0].title, "Computer_Networks");
        books[0].available = 1;

        books[1].id = 2;
        strcpy(books[1].title, "Operating_Systems");
        books[1].available = 1;

        books[2].id = 3;
        strcpy(books[2].title, "C_Programming");
        books[2].available = 1;

        count = 3;
        return;
    }

    while (count < MAX_BOOKS &&
           fscanf(fp, "%d %49s %d",
                  &books[count].id,
                  books[count].title,
                  &books[count].available) == 3)
    {
        count++;
    }

    fclose(fp);
}

/* Save books to file */
void saveBooks()
{
    FILE *fp = fopen("library.txt", "w");

    if (fp == NULL)
        return;

    for (int i = 0; i < count; i++)
    {
        fprintf(fp, "%d %s %d\n",
                books[i].id,
                books[i].title,
                books[i].available);
    }

    fclose(fp);
}

/* Log activity */
void logActivity(char *text)
{
    FILE *fp = fopen("library.log", "a");

    if (fp == NULL)
        return;

    time_t now = time(NULL);

    fprintf(fp, "%s - %s\n", ctime(&now), text);

    fclose(fp);
}

/* Process client request */
void processRequest(char *request, char *response)
{
    char operation[20];
    int id;

    if (sscanf(request, "%19s", operation) != 1)
    {
        strcpy(response, "Invalid request.");
        return;
    }

    /* LIST */
    if (strcmp(operation, "LIST") == 0)
    {
        strcpy(response, "\nAvailable Books\n");
        strcat(response, "----------------------------\n");

        int found = 0;

        for (int i = 0; i < count; i++)
        {
            if (books[i].available)
            {
                char line[100];

                sprintf(line,
                        "ID : %d   %s\n",
                        books[i].id,
                        books[i].title);

                strcat(response, line);
                found = 1;
            }
        }

        if (!found)
            strcat(response, "No books available.\n");

        logActivity("Viewed available books");
        return;
    }

    /* SEARCH / ISSUE / RETURN */
    if (sscanf(request, "%19s %d", operation, &id) != 2)
    {
        strcpy(response, "Invalid request.");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        if (books[i].id == id)
        {
            /* SEARCH */
            if (strcmp(operation, "SEARCH") == 0)
            {
                sprintf(response,
                        "\nBook Details\n"
                        "----------------------------\n"
                        "ID     : %d\n"
                        "Title  : %s\n"
                        "Status : %s",
                        books[i].id,
                        books[i].title,
                        books[i].available ?
                        "Available" : "Issued");

                logActivity("Book searched");
                return;
            }

            /* ISSUE */
            if (strcmp(operation, "ISSUE") == 0)
            {
                if (books[i].available)
                {
                    books[i].available = 0;
                    saveBooks();

                    sprintf(response,
                            "Book '%s' issued successfully.",
                            books[i].title);

                    logActivity("Book issued");
                }
                else
                {
                    strcpy(response, "Book is already issued.");
                }

                return;
            }

            /* RETURN */
            if (strcmp(operation, "RETURN") == 0)
            {
                if (!books[i].available)
                {
                    books[i].available = 1;
                    saveBooks();

                    sprintf(response,
                            "Book '%s' returned successfully.",
                            books[i].title);

                    logActivity("Book returned");
                }
                else
                {
                    strcpy(response, "Book was not issued.");
                }

                return;
            }

            strcpy(response, "Invalid operation.");
            return;
        }
    }

    strcpy(response, "Book ID not found.");
}

int main()
{
    int sockfd;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    char request[SIZE];
    char response[SIZE];

    loadBooks();

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Socket error");
        return 1;
    }

    memset(&server, 0, sizeof(server));

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
    printf("       UDP LIBRARY SERVER\n");
    printf("====================================\n");
    printf("Port : %d\n", PORT);
    printf("Server is ready...\n");

    while (1)
    {
        memset(request, 0, sizeof(request));
        memset(response, 0, sizeof(response));

        int n = recvfrom(sockfd,
                         request,
                         SIZE - 1,
                         0,
                         (struct sockaddr *)&client,
                         &len);

        if (n < 0)
            continue;

        request[n] = '\0';

        printf("\nRequest : %s\n", request);

        processRequest(request, response);

        sendto(sockfd,
               response,
               strlen(response) + 1,
               0,
               (struct sockaddr *)&client,
               len);

        printf("Response sent.\n");
    }

    close(sockfd);

    return 0;
}
