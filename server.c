
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8084
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

struct client_info
{
    int id;                      // position dans clients[] + 1 
    int socket;                  // -1 = case libre / client déconnecté 
    struct sockaddr_in addr;
};

struct client_info clients[MAX_CLIENTS];
int serverfd;

// Thread de réception : un par client, il affiche ses messages
void *recevoir(void *arg)
{
    struct client_info *c = (struct client_info *)arg;
    char msg[BUFFER_SIZE];
    char ip_str[INET_ADDRSTRLEN];
    int n;

    inet_ntop(AF_INET, &c->addr.sin_addr, ip_str, sizeof(ip_str));

    while ((n = recv(c->socket, msg, sizeof(msg) - 1, 0)) > 0)
    {
        msg[n] = '\0';
        msg[strcspn(msg, "\r\n")] = '\0';

        printf("[Client %d | %s:%u] %s\n",
               c->id, ip_str, (unsigned int)ntohs(c->addr.sin_port), msg);
        fflush(stdout);
    }

    // Déconnecté : la case est libérée, les autres ne bougent pas
    printf("Client %d disconnected.\n", c->id);
    fflush(stdout);
    close(c->socket);
    c->socket = -1;
    return NULL;
}

// Thread d'acceptation : attend les nouveaux clients
void *accepter(void *arg)
{
    (void)arg;

    while (1)
    {
        struct sockaddr_in sender;
        socklen_t sender_len = sizeof(sender);

        int cs = accept(serverfd, (struct sockaddr *)&sender, &sender_len);

        if (cs == -1)
        {
            perror("accept");
            continue;
        }

        int id = -1;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].socket == -1)
            {
                id = i;
                break;
            }
        }

        if (id == -1)
        {
            printf("Maximum number of clients reached.\n");
            close(cs);
            continue;
        }

        clients[id].socket = cs;
        clients[id].addr = sender;
        printf("New client registered: ID %d\n", id + 1);
        fflush(stdout);

        pthread_t t;
        pthread_create(&t, NULL, recevoir, &clients[id]);
        pthread_detach(t);
    }

    return NULL;
}

int main(void)
{
    struct sockaddr_in server_addr;
    char command[BUFFER_SIZE];

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].id = i + 1;
        clients[i].socket = -1;
    }

    serverfd = socket(AF_INET, SOCK_STREAM, 0);

    if (serverfd == -1)
    {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverfd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(serverfd);
        return 1;
    }

    if (listen(serverfd, MAX_CLIENTS) == -1)
    {
        perror("listen");
        close(serverfd);
        return 1;
    }

    printf("TCP server listening on port %d\n", PORT);
    printf("Waiting for clients...\n");
    printf("Reply format: ID message  (quit to stop)\n\n");

    pthread_t t;
    pthread_create(&t, NULL, accepter, NULL);

    //Thread principal : on tape "ID message" pour répondre
    while (1)
    {
        if (fgets(command, sizeof(command), stdin) == NULL)
        {
            printf("\nStopping server.\n");
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "quit") == 0)
        {
            printf("Stopping server.\n");
            break;
        }

        char *endptr;
        long id = strtol(command, &endptr, 10);

        if (endptr == command ||
            (*endptr != ' ' && *endptr != '\t'))
        {
            printf("Invalid command. Use: ID message\n");
            continue;
        }

        while (*endptr == ' ' || *endptr == '\t')
            endptr++;

        if (id < 1 || id > MAX_CLIENTS)
        {
            printf("Unknown client ID.\n");
            continue;
        }

        if (*endptr == '\0')
        {
            printf("Message cannot be empty.\n");
            continue;
        }

        int target = (int)id - 1;

        if (clients[target].socket == -1)
        {
            printf("Client %ld is not connected.\n", id);
            continue;
        }

        int sent = send(clients[target].socket,
                        endptr,
                        strlen(endptr),
                        MSG_NOSIGNAL);

        if (sent == -1)
            perror("send");
        else
            printf("Reply sent to client %ld.\n", id);
    }

    close(serverfd);
    return 0;
}