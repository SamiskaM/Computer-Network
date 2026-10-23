#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8085
#define MAX_CLIENTS 20
#define MAX_QUESTIONS 5

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

struct User
{
    char username[30];
    char password[30];
};

struct Question
{
    char question[200];
    char optionA[50];
    char optionB[50];
    char optionC[50];
    char optionD[50];
    char answer;
};

struct User users[] =
{
    {"student1", "1234"},
    {"student2", "5678"},
    {"student3", "abcd"}
};

struct Question questions[MAX_QUESTIONS] =
{
    {
        "Which protocol is connection-oriented?",
        "UDP",
        "TCP",
        "IP",
        "ARP",
        'B'
    },
    {
        "Which layer provides end-to-end communication?",
        "Physical",
        "Data Link",
        "Transport",
        "Network",
        'C'
    },
    {
        "Which device connects different networks?",
        "Hub",
        "Switch",
        "Router",
        "Repeater",
        'C'
    },
    {
        "Which protocol is used to transfer web pages?",
        "HTTP",
        "FTP",
        "SMTP",
        "ARP",
        'A'
    },
    {
        "Which command is used to test network connectivity?",
        "ping",
        "copy",
        "mkdir",
        "delete",
        'A'
    }
};

void log_activity(char *username, char *activity)
{
    FILE *fp;
    time_t now;
    char *time_string;

    pthread_mutex_lock(&file_mutex);

    fp = fopen("activity.log", "a");

    if (fp != NULL)
    {
        time(&now);
        time_string = ctime(&now);

        time_string[strcspn(time_string, "\n")] = '\0';

        fprintf(fp, "[%s] User: %s - %s\n",
                time_string, username, activity);

        fclose(fp);
    }

    pthread_mutex_unlock(&file_mutex);
}

int authenticate(char *username, char *password)
{
    int i;
    int total_users = sizeof(users) / sizeof(users[0]);

    for (i = 0; i < total_users; i++)
    {
        if (strcmp(username, users[i].username) == 0 &&
            strcmp(password, users[i].password) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void send_questions(int client_socket)
{
    char buffer[1000];
    int i;

    for (i = 0; i < MAX_QUESTIONS; i++)
    {
        sprintf(buffer,
                "\nQuestion %d: %s\n"
                "A. %s\n"
                "B. %s\n"
                "C. %s\n"
                "D. %s\n",
                i + 1,
                questions[i].question,
                questions[i].optionA,
                questions[i].optionB,
                questions[i].optionC,
                questions[i].optionD);

        send(client_socket, buffer, strlen(buffer), 0);
    }

    strcpy(buffer, "END_QUESTIONS");
    send(client_socket, buffer, strlen(buffer) + 1, 0);
}

void *client_handler(void *arg)
{
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[1024];
    char username[30];
    char password[30];

    int score = 0;
    int i;

    pthread_detach(pthread_self());

    /* Receive username */
    memset(buffer, 0, sizeof(buffer));

    if (recv(client_socket, buffer, sizeof(buffer) - 1, 0) <= 0)
    {
        close(client_socket);
        return NULL;
    }

    strcpy(username, buffer);

    /* Tell client username was received */
    strcpy(buffer, "USERNAME_OK");
    send(client_socket, buffer, strlen(buffer) + 1, 0);

    /* Receive password */
    memset(buffer, 0, sizeof(buffer));

    if (recv(client_socket, buffer, sizeof(buffer) - 1, 0) <= 0)
    {
        close(client_socket);
        return NULL;
    }

    strcpy(password, buffer);

    /* Authentication */
    if (authenticate(username, password))
    {
        strcpy(buffer, "LOGIN_SUCCESS");
        send(client_socket, buffer, strlen(buffer) + 1, 0);

        printf("User %s logged in.\n", username);

        log_activity(username, "Logged in successfully");
    }
    else
    {
        strcpy(buffer, "LOGIN_FAILED");
        send(client_socket, buffer, strlen(buffer) + 1, 0);

        printf("Login failed for user %s.\n", username);

        log_activity(username, "Login failed");

        close(client_socket);
        return NULL;
    }

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        if (recv(client_socket, buffer, sizeof(buffer) - 1, 0) <= 0)
        {
            log_activity(username, "Client disconnected");
            break;
        }

        if (strcmp(buffer, "1") == 0)
        {
            send_questions(client_socket);

            log_activity(username, "Requested examination questions");
        }

        else if (strcmp(buffer, "2") == 0)
        {
            score = 0;

            for (i = 0; i < MAX_QUESTIONS; i++)
            {
                memset(buffer, 0, sizeof(buffer));

                if (recv(client_socket,
                         buffer,
                         sizeof(buffer) - 1,
                         0) <= 0)
                {
                    log_activity(username,
                                 "Disconnected while submitting answers");

                    close(client_socket);
                    return NULL;
                }

                if (buffer[0] == questions[i].answer)
                {
                    score++;
                }
            }

            sprintf(buffer,
                    "EXAM_COMPLETED\nYour Score = %d/%d",
                    score, MAX_QUESTIONS);

            send(client_socket, buffer, strlen(buffer) + 1, 0);

            log_activity(username, "Submitted examination");
        }

        else if (strcmp(buffer, "3") == 0)
        {
            sprintf(buffer,
                    "Your Final Score = %d/%d",
                    score, MAX_QUESTIONS);

            send(client_socket, buffer, strlen(buffer) + 1, 0);

            log_activity(username, "Viewed final score");
        }

        else if (strcmp(buffer, "0") == 0)
        {
            strcpy(buffer, "GOODBYE");
            send(client_socket, buffer, strlen(buffer) + 1, 0);

            log_activity(username, "Logged out");

            break;
        }

        else
        {
            strcpy(buffer, "INVALID_REQUEST");
            send(client_socket, buffer, strlen(buffer) + 1, 0);

            log_activity(username, "Sent invalid request");
        }
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

    pthread_t thread;

    printf("====================================\n");
    printf("   ONLINE EXAMINATION SERVER\n");
    printf("====================================\n");

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    int option = 1;

    setsockopt(server_socket,
               SOL_SOCKET,
               SO_REUSEADDR,
               &option,
               sizeof(option));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket,
             (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        exit(1);
    }

    printf("Port : %d\n", PORT);
    printf("Questions : %d\n", MAX_QUESTIONS);
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

        printf("New client connected.\n");

        int *new_socket = malloc(sizeof(int));

        if (new_socket == NULL)
        {
            close(client_socket);
            continue;
        }

        *new_socket = client_socket;

        if (pthread_create(&thread,
                           NULL,
                           client_handler,
                           new_socket) != 0)
        {
            perror("Thread creation failed");
            close(client_socket);
            free(new_socket);
        }
    }

    close(server_socket);

    return 0;
}
