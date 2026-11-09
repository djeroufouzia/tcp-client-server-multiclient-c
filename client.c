#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_IP "192.168.56.7"   /* <-- mets ici l'adresse du serveur */
#define PORT 8084
#define BUFFER_SIZE 1024

int sockfd;

/* Thread de réception : affiche les réponses du serveur. */
void *recevoir(void *arg)
{
    (void)arg;
    char msg[BUFFER_SIZE];
    int n;

    while ((n = recv(sockfd, msg, sizeof(msg) - 1, 0)) > 0)
    {
        msg[n] = '\0';
        printf("\n[Server] %s\n", msg);
        fflush(stdout);
    }

    printf("\nServer disconnected.\n");
    exit(0);
}

int main(void)
{
    struct sockaddr_in server_addr;
    char command[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) != 1)
    {
        printf("Invalid IP address.\n");
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1)
    {
        perror("connect");
        close(sockfd);
        return 1;
    }

    printf("Connected to %s:%d (type quit to leave)\n\n", SERVER_IP, PORT);

    pthread_t t;
    pthread_create(&t, NULL, recevoir, NULL);

    /* Thread principal : on tape un message et on l'envoie. */
    while (fgets(command, sizeof(command), stdin) != NULL)
    {
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "quit") == 0)
            break;

        if (*command == '\0')
            continue;

        if (send(sockfd, command, strlen(command), MSG_NOSIGNAL) == -1)
        {
            perror("send");
            break;
        }
    }

    close(sockfd);
    return 0;
}