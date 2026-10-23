#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int clientSocket;

    struct sockaddr_in serverAddress;

    char input[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    /* Create socket */
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    /* Server address */
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Connect to server */
    if (connect(clientSocket,
                (struct sockaddr *)&serverAddress,
                sizeof(serverAddress)) < 0)
    {
        perror("Connection failed");
        close(clientSocket);
        return 1;
    }

    printf("\n========================================\n");
    printf("        PALINDROME TCP CLIENT\n");
    printf("========================================\n");

    printf("Enter text : ");
    fgets(input, sizeof(input), stdin);

    input[strcspn(input, "\n")] = '\0';

    /* Send text to server */
    send(clientSocket, input, strlen(input) + 1, 0);

    /* Receive result */
    memset(response, 0, sizeof(response));

    recv(clientSocket, response, sizeof(response) - 1, 0);

    printf("\n------------- RESULT -------------------\n");
    printf("Input    : %s\n", input);
    printf("Server   : %s\n", response);
    printf("----------------------------------------\n");

    close(clientSocket);

    printf("Connection closed.\n");
    printf("========================================\n");

    return 0;
