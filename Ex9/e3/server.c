#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define PORT 8086
#define BUFFER_SIZE 1024

/* Send exactly size bytes */
int send_all(int socket_fd, const void *data, int size)
{
    int total = 0;
    int bytes;

    while (total < size)
    {
        bytes = send(socket_fd, (char *)data + total, size - total, 0);

        if (bytes <= 0)
            return 0;

        total += bytes;
    }

    return 1;
}

/* Receive exactly size bytes */
int recv_all(int socket_fd, void *data, int size)
{
    int total = 0;
    int bytes;

    while (total < size)
    {
        bytes = recv(socket_fd, (char *)data + total, size - total, 0);

        if (bytes <= 0)
            return 0;

        total += bytes;
    }

    return 1;
}

/* Create sample files */
void create_files()
{
    FILE *fp;

    fp = fopen("file1.txt", "r");

    if (fp == NULL)
    {
        fp = fopen("file1.txt", "w");

        fprintf(fp,
                "This is File 1.\n"
                "Welcome to the TCP File Sharing Server.\n"
                "Computer Networks Lab.\n");

        fclose(fp);
    }
    else
    {
        fclose(fp);
    }

    fp = fopen("file2.txt", "r");

    if (fp == NULL)
    {
        fp = fopen("file2.txt", "w");

        fprintf(fp,
                "This is File 2.\n"
                "This file is shared using TCP.\n"
                "Concurrent File Sharing Server.\n");

        fclose(fp);
    }
    else
    {
        fclose(fp);
    }

    fp = fopen("file3.txt", "r");

    if (fp == NULL)
    {
        fp = fopen("file3.txt", "w");

        fprintf(fp,
                "This is File 3.\n"
                "Computer Networks Laboratory.\n"
                "TCP File Transfer Example.\n");

        fclose(fp);
    }
    else
    {
        fclose(fp);
    }
}

/* Send available file list */
void send_file_list(int client_socket)
{
    char file_list[1024];

    strcpy(file_list,
           "\n------------- FILE LIST -------------\n"
           "Available Files:\n"
           "1. file1.txt\n"
           "2. file2.txt\n"
           "3. file3.txt\n"
           "-------------------------------------\n");

    send_all(client_socket, file_list, sizeof(file_list));
}

/* Handle one client */
void *client_handler(void *arg)
{
    int client_socket = *(int *)arg;

    free(arg);

    pthread_detach(pthread_self());

    char choice[20];
    char filename[100];
    char status[1024];

    /* Receive choice */
    if (!recv_all(client_socket, choice, sizeof(choice)))
    {
        close(client_socket);
        return NULL;
    }

    /* Option 1 - List files */
    if (strcmp(choice, "1") == 0)
    {
        send_file_list(client_socket);
    }

    /* Option 2 - Download file */
    else if (strcmp(choice, "2") == 0)
    {
        if (!recv_all(client_socket, filename, sizeof(filename)))
        {
            close(client_socket);
            return NULL;
        }

        FILE *fp = fopen(filename, "rb");

        if (fp == NULL)
        {
            memset(status, 0, sizeof(status));
            strcpy(status, "FILE_NOT_FOUND");

            send_all(client_socket, status, sizeof(status));

            close(client_socket);
            return NULL;
        }

        /* Send file found message */
        memset(status, 0, sizeof(status));
        strcpy(status, "FILE_FOUND");

        send_all(client_socket, status, sizeof(status));

        /* Find file size */
        fseek(fp, 0, SEEK_END);

        long long file_size = ftell(fp);

        rewind(fp);

        /* Send file size */
        send_all(client_socket, &file_size, sizeof(file_size));

        /* Send file data */
        char buffer[BUFFER_SIZE];

        long long total_sent = 0;

        while (total_sent < file_size)
        {
            int remaining = file_size - total_sent;

            int bytes_to_send;

            if (remaining > BUFFER_SIZE)
                bytes_to_send = BUFFER_SIZE;
            else
                bytes_to_send = remaining;

            int bytes_read = fread(buffer, 1, bytes_to_send, fp);

            if (bytes_read <= 0)
                break;

            if (!send_all(client_socket, buffer, bytes_read))
                break;

            total_sent += bytes_read;
        }

        fclose(fp);

        printf("File sent: %s\n", filename);
    }

    /* Invalid request */
    else
    {
        memset(status, 0, sizeof(status));
        strcpy(status, "INVALID_REQUEST");

        send_all(client_socket, status, sizeof(status));
    }

    close(client_socket);

    return NULL;
}

int main()
{
    int server_socket;
    int client_socket;

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    socklen_t client_length;

    /* Create sample files */
    create_files();

    /* Create socket */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    /* Allow port reuse */
    int option = 1;

    setsockopt(server_socket,
               SOL_SOCKET,
               SO_REUSEADDR,
               &option,
               sizeof(option));

    /* Server address */
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    /* Bind */
    if (bind(server_socket,
             (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    /* Listen */
    if (listen(server_socket, 10) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }

    printf("\n====================================\n");
    printf("       TCP FILE SHARING SERVER\n");
    printf("====================================\n");
    printf("Port : %d\n", PORT);
    printf("Server is ready...\n\n");

    while (1)
    {
        client_length = sizeof(client_address);

        client_socket = accept(server_socket,
                               (struct sockaddr *)&client_address,
                               &client_length);

        if (client_socket < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");

        /* Allocate socket for new thread */
        int *new_socket = malloc(sizeof(int));

        if (new_socket == NULL)
        {
            close(client_socket);
            continue;
        }

        *new_socket = client_socket;

        pthread_t thread;

        if (pthread_create(&thread,
                           NULL,
                           client_handler,
                           new_socket) != 0)
        {
            perror("Thread creation failed");

            free(new_socket);
            close(client_socket);
        }
    }

    close(server_socket);

    return 0;
}
