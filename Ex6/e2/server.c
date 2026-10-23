#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int isPalindrome(char text[])
{
    int start = 0;
    int end = strlen(text) - 1;

    while (start < end)
    {
        if (tolower(text[start]) != tolower(text[end]))
            return 0;

        start++;
        end--;
    }

    return 1;
}

int main()
{
    int serverSocket, clientSocket;
    int option = 1;

    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    socklen_t addressLength = sizeof(clientAddress);

    char text[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    /* Create socket */
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    /* Allow port reuse */
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
                   &option, sizeof(option)) < 0)
    {
        perror("setsockopt failed");
        close(serverSocket);
        return 1;
    }

    /* Server address */
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    /* Bind */
    if (bind(serverSocket,
             (struct sockaddr *)&serverAddress,
             sizeof(serverAddress)) < 0)
    {
        perror("Bind failed");
        close(serverSocket);
        return 1;
    }

    /* Listen */
    if (listen(serverSocket, 5) < 0)
    {
        perror("Listen failed");
        close(serverSocket);
        return 1;
    }

    printf("\n========================================\n");
    printf("        PALINDROME TCP SERVER\n");
    printf("========================================\n");
    printf("Port       : %d\n", PORT);
    printf("Status     : Waiting for client...\n");
    printf("----------------------------------------\n");

    /* Accept client */
    clientSocket = accept(serverSocket,
                          (struct sockaddr *)&clientAddress,
                          &addressLength);

    if (clientSocket < 0)
    {
        perror("Accept failed");
        close(serverSocket);
        return 1;
    }

    printf("Client connected successfully.\n");

    /* Receive data */
    memset(text, 0, sizeof(text));

    if (recv(clientSocket, text, sizeof(text) - 1, 0) <= 0)
    {
        printf("Failed to receive data.\n");
        close(clientSocket);
        close(serverSocket);
        return 1;
    }

    text[strcspn(text, "\r\n")] = '\0';

    printf("Received text : %s\n", text);

    /* Check palindrome */
    if (isPalindrome(text))
    {
        strcpy(result, "PALINDROME");
    }
    else
    {
        strcpy(result, "NOT A PALINDROME");
    }

    /* Send result */
    send(clientSocket, result, strlen(result) + 1, 0);

    printf("Result        : %s\n", result);
    printf("Response sent successfully.\n");

    printf("----------------------------------------\n");
    printf("Client disconnected.\n");
    printf("Server shutting down.\n");
    printf("========================================\n");

    close(clientSocket);
    close(serverSocket);

    return 0;
}
