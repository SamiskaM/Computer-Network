#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

int main()
{
    int client_socket;

    struct sockaddr_in server_address;

    char choice[20];
    char filename[100];
    char status[1024];

    /* Create socket */
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    /* Server address */
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1",
              &server_address.sin_addr);

    /* Connect */
    if (connect(client_socket,
                (struct sockaddr *)&server_address,
                sizeof(server_address)) < 0)
    {
        perror("Connection failed");
        close(client_socket);
        return 1;
    }

    printf("\n====================================\n");
    printf("        TCP FILE SHARING CLIENT\n");
    printf("====================================\n");

    printf("\n1. View Available Files\n");
    printf("2. Download File\n");
    printf("0. Exit\n");

    printf("\nEnter choice : ");
    scanf("%19s", choice);

    /* Exit */
    if (strcmp(choice, "0") == 0)
    {
        close(client_socket);
        return 0;
    }

    /* Send choice */
    if (!send_all(client_socket, choice, sizeof(choice)))
    {
        printf("Failed to send request.\n");
        close(client_socket);
        return 1;
    }

    /* View file list */
    if (strcmp(choice, "1") == 0)
    {
        char file_list[1024];

        if (!recv_all(client_socket,
                      file_list,
                      sizeof(file_list)))
        {
            printf("Failed to receive file list.\n");
            close(client_socket);
            return 1;
        }

        printf("%s", file_list);
    }

    /* Download file */
    else if (strcmp(choice, "2") == 0)
    {
        printf("Enter file name : ");
        scanf("%99s", filename);

        /* Send filename */
        if (!send_all(client_socket,
                      filename,
                      sizeof(filename)))
        {
            printf("Failed to send filename.\n");
            close(client_socket);
            return 1;
        }

        /* Receive status */
        if (!recv_all(client_socket,
                      status,
                      sizeof(status)))
        {
            printf("Failed to receive server response.\n");
            close(client_socket);
            return 1;
        }

        /* File does not exist */
        if (strcmp(status, "FILE_NOT_FOUND") == 0)
        {
            printf("\nFile not found on server.\n");
        }

        /* File exists */
        else if (strcmp(status, "FILE_FOUND") == 0)
        {
            long long file_size;
            long long total_received = 0;

            /* Receive file size */
            if (!recv_all(client_socket,
                          &file_size,
                          sizeof(file_size)))
            {
                printf("Failed to receive file size.\n");
                close(client_socket);
                return 1;
            }

            printf("\nDownloading %s...\n", filename);
            printf("File size : %lld bytes\n", file_size);

            FILE *fp = fopen(filename, "wb");

            if (fp == NULL)
            {
                printf("Cannot create output file.\n");
                close(client_socket);
                return 1;
            }

            char buffer[BUFFER_SIZE];

            /* Receive exactly file_size bytes */
            while (total_received < file_size)
            {
                long long remaining =
                    file_size - total_received;

                int bytes_to_receive;

                if (remaining > BUFFER_SIZE)
                    bytes_to_receive = BUFFER_SIZE;
                else
                    bytes_to_receive = remaining;

                int bytes_received =
                    recv(client_socket,
                         buffer,
                         bytes_to_receive,
                         0);

                if (bytes_received <= 0)
                {
                    printf("File transfer interrupted.\n");
                    fclose(fp);
                    close(client_socket);
                    return 1;
                }

                fwrite(buffer,
                       1,
                       bytes_received,
                       fp);

                total_received += bytes_received;
            }

            fclose(fp);

            printf("File downloaded successfully.\n");
        }

        else
        {
            printf("\nInvalid server response.\n");
        }
    }

    else
    {
        if (recv_all(client_socket,
                     status,
                     sizeof(status)))
        {
            printf("\nInvalid choice.\n");
        }
    }

    close(client_socket);

    return 0;
}
