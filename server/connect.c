#include "main.h"

result init_server(tftp_server *server)
{
    printf("TFTP: Server is waiting...\n");
    server->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server->sock_fd < 0)
    {
        perror("socket");
        return FAILURE;
    }

    memset(&server->server_addr, 0, sizeof(server->server_addr));
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_port = htons(TFTP_SERVER_PORT);
    server->server_addr.sin_addr.s_addr = inet_addr(TFTP_SERVER_IP);

    if (bind(server->sock_fd, (struct sockaddr *)&server->server_addr, sizeof(server->server_addr)) < 0)
    {
        perror("bind");
        return FAILURE;
    }

    server->client_len = sizeof(server->client_addr);
    server->transfer_port = START_CHILD_PORT;

    return SUCCESS;
}

void handle_new_client(tftp_server *server)
{
    pid_t pid;
    int child_sock;
    int port = server->transfer_port++;

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        /* CHILD PROCESS */

        close(server->sock_fd); // child doesn't use port 6000

        child_sock = bind_child_socket(port);
        if (child_sock < 0)
            exit(1);

        printf(BLUE BOLD "INFO : Client connected on port %d" COLOREND "\n\n", port);

        child_send_ack(child_sock, server);

        close(child_sock);
        exit(0);
    }

    /* PARENT RETURNS TO LISTENING */
}

int bind_child_socket(int port)
{
    int sockfd;
    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void child_send_ack(int sockfd, tftp_server *server)
{
    tftp_packet ack;

    memset(&ack, 0, sizeof(ack));
    ack.opcode = htons(OP_ACK);
    ack.ack.block = htons(0);

    sendto(sockfd, &ack, 4, 0, (struct sockaddr *)&server->client_addr, server->client_len);
}
