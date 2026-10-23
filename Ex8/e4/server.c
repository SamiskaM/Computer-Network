#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8084
#define MAX 100
#define SIZE 2048

struct Book
{
    int id;
    char title[50];
    char author[50];
    char category[30];
    int available;
};

struct Book books[MAX];
int count = 0;

/* Function declaration */
void saveBooks();

/* Load books from file */
void loadBooks()
{
    FILE *fp;

    fp = fopen("library.txt", "r");

    /* If file does not exist, create default books */
    if (fp == NULL)
    {
        count = 4;

        books[0].id = 1;
        strcpy(books[0].title, "Computer_Networks");
        strcpy(books[0].author, "Kurose");
        strcpy(books[0].category, "Networking");
        books[0].available = 1;

        books[1].id = 2;
        strcpy(books[1].title, "Operating_Systems");
        strcpy(books[1].author, "Galvin");
        strcpy(books[1].category, "OS");
        books[1].available = 1;

        books[2].id = 3;
        strcpy(books[2].title, "C_Programming");
        strcpy(books[2].author, "DennisC");
        strcpy(books[2].category, "Programming");
        books[2].available = 1;

        books[3].id = 4;
        strcpy(books[3].title, "Database_Systems");
        strcpy(books[3].author, "Navathe");
        strcpy(books[3].category, "Database");
        books[3].available = 1;

        saveBooks();
        return;
    }

    while (count < MAX &&
           fscanf(fp, "%d %49s %49s %29s %d",
                  &books[count].id,
                  books[count].title,
                  books[count].author,
                  books[count].category,
                  &books[count].available) == 5)
    {
        count++;
    }

    fclose(fp);
}

/* Save books to file */
void saveBooks()
{
    FILE *fp;

    fp = fopen("library.txt", "w");

    if (fp == NULL)
    {
        printf("Unable to save library data.\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(fp, "%d %s %s %s %d\n",
                books[i].id,
                books[i].title,
                books[i].author,
                books[i].category,
                books[i].available);
    }

    fclose(fp);
}

/* Process client request */
void processRequest(char *request, char *response)
{
    int choice;
    int id;
    char key[50];

    /* Get menu choice */
    if (sscanf(request, "%d", &choice) != 1)
    {
        strcpy(response, "Invalid request.");
        return;
    }

    /* Search by title */
    if (choice == 1)
    {
        if (sscanf(request, "%d %49s", &choice, key) != 2)
        {
            strcpy(response, "Invalid title.");
            return;
        }

        for (int i = 0; i < count; i++)
        {
            if (strcasecmp(books[i].title, key) == 0)
            {
                sprintf(response,
                        "ID        : %d\n"
                        "Title     : %s\n"
                        "Author    : %s\n"
                        "Category  : %s\n"
                        "Available : %s",
                        books[i].id,
                        books[i].title,
                        books[i].author,
                        books[i].category,
                        books[i].available ? "Yes" : "No");

                return;
            }
        }

        strcpy(response, "Book not found.");
    }

    /* Search by author */
    else if (choice == 2)
    {
        if (sscanf(request, "%d %49s", &choice, key) != 2)
        {
            strcpy(response, "Invalid author.");
            return;
        }

        for (int i = 0; i < count; i++)
        {
            if (strcasecmp(books[i].author, key) == 0)
            {
                sprintf(response,
                        "ID        : %d\n"
                        "Title     : %s\n"
                        "Author    : %s\n"
                        "Category  : %s\n"
                        "Available : %s",
                        books[i].id,
                        books[i].title,
                        books[i].author,
                        books[i].category,
                        books[i].available ? "Yes" : "No");

                return;
            }
        }

        strcpy(response, "Book not found.");
    }

    /* Issue book */
    else if (choice == 3)
    {
        if (sscanf(request, "%d %d", &choice, &id) != 2)
        {
            strcpy(response, "Invalid book ID.");
            return;
        }

        for (int i = 0; i < count; i++)
        {
            if (books[i].id == id)
            {
                if (books[i].available == 1)
                {
                    books[i].available = 0;
                    saveBooks();

                    strcpy(response,
                           "Book issued successfully.");
                }
                else
                {
                    strcpy(response,
                           "Book is not available.");
                }

                return;
            }
        }

        strcpy(response, "Book not found.");
    }

    /* Return book */
    else if (choice == 4)
    {
        if (sscanf(request, "%d %d", &choice, &id) != 2)
        {
            strcpy(response, "Invalid book ID.");
            return;
        }

        for (int i = 0; i < count; i++)
        {
            if (books[i].id == id)
            {
                if (books[i].available == 0)
                {
                    books[i].available = 1;
                    saveBooks();

                    strcpy(response,
                           "Book returned successfully.");
                }
                else
                {
                    strcpy(response,
                           "Book is already available.");
                }

                return;
            }
        }

        strcpy(response, "Book not found.");
    }

    /* View book details */
    else if (choice == 5)
    {
        if (sscanf(request, "%d %d", &choice, &id) != 2)
        {
            strcpy(response, "Invalid book ID.");
            return;
        }

        for (int i = 0; i < count; i++)
        {
            if (books[i].id == id)
            {
                sprintf(response,
                        "ID        : %d\n"
                        "Title     : %s\n"
                        "Author    : %s\n"
                        "Category  : %s\n"
                        "Available : %s",
                        books[i].id,
                        books[i].title,
                        books[i].author,
                        books[i].category,
                        books[i].available ? "Yes" : "No");

                return;
            }
        }

        strcpy(response, "Book not found.");
    }

    else
    {
        strcpy(response, "Invalid choice.");
    }
}

int main()
{
    int server_fd;
    int client_fd;
    int option = 1;

    struct sockaddr_in server;

    /* Load library */
    loadBooks();

    /* Create TCP socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket");
        return 1;
    }

    /* Reuse port */
    setsockopt(server_fd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &option,
               sizeof(option));

    /* Server address */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    /* Bind */
    if (bind(server_fd,
             (struct sockaddr *)&server,
             sizeof(server)) < 0)
    {
        perror("Bind");
        close(server_fd);
        return 1;
    }

    /* Listen */
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen");
        close(server_fd);
        return 1;
    }

    printf("\n====================================\n");
    printf("          LIBRARY SERVER\n");
    printf("====================================\n");
    printf("Port : %d\n", PORT);
    printf("Books loaded : %d\n", count);
    printf("Server is ready...\n");

    /* Iterative TCP server */
    while (1)
    {
        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0)
            continue;

        char request[SIZE] = {0};
        char response[SIZE] = {0};

        int n = recv(client_fd,
                     request,
                     SIZE - 1,
                     0);

        if (n > 0)
        {
            request[n] = '\0';

            processRequest(request, response);

            send(client_fd,
                 response,
                 strlen(response),
                 0);

            printf("Request processed.\n");
        }

        close(client_fd);
    }

    close(server_fd);

    return 0;
}
