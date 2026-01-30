#include "main.h"

void handle_wrq(tftp_packet *req, tftp_server *server)
{
    int port = server->transfer_port++; // Increment in parent BEFORE fork
    pid_t pid = fork();

    if (pid == 0) // CHILD
    {
        int data_sock;
        struct sockaddr_in data_addr;

        data_sock = socket(AF_INET, SOCK_DGRAM, 0);

        memset(&data_addr, 0, sizeof(data_addr));
        data_addr.sin_family = AF_INET;
        data_addr.sin_addr.s_addr = inet_addr(TFTP_SERVER_IP);
        data_addr.sin_port = htons(port);

        bind(data_sock, (struct sockaddr *)&data_addr, sizeof(data_addr));

        printf(GRAY BOLD "INFO : [SERVER] PUT file processing on port %d" COLOREND "\n", port);

        handle_put_file(data_sock, req, &server->client_addr, server->client_len);

        close(data_sock);
        exit(0);
    }
}

void handle_put_file(int sockfd, tftp_packet *req, struct sockaddr_in *client, socklen_t client_len)
{
    int fd;
    uint16_t expected_block = 1;
    tftp_packet pkt;

    printf(YELLOW "PUT request for file: %s" COLOREND "\n", req->request.filename);

    if (access(req->request.filename, F_OK) == 0)
    {
        send_error(sockfd, client, client_len, 6, "File exists on server");

        int bytes = recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)client, &client_len);

        if (bytes < 0)
        {
            printf(RED "INFO : Timeout / client disconnected" COLOREND "\n\n");
            return;
        }
    }

    /* WRQ handshake */
    send_ack(sockfd, client, client_len, 0);

    fd = open(req->request.filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
    {
        send_error(sockfd, client, client_len, 2, "Access violation");
        return;
    }

    set_timeout(sockfd);

    while (1)
    {
        int bytes = recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)client, &client_len);

        if (bytes < 0)
        {
            printf(RED "INFO : Timeout / client disconnected" COLOREND "\n\n");
            break;
        }

        if (ntohs(pkt.opcode) != OP_DATA)
            continue;

        uint16_t block = ntohs(pkt.data_pkt.block);

        if (block == expected_block)
        {
            int data_len = bytes - 4;
            write(fd, pkt.data_pkt.data, data_len);

            send_ack(sockfd, client, client_len, block);

            if (data_len < DATA_SIZE)
                break;

            expected_block++;
        }
        else
        {
            send_ack(sockfd, client, client_len, expected_block - 1);
        }
    }

    close(fd);
    printf(BLUE BOLD "File received successfully" COLOREND "\n\n");
}

void send_ack(int sockfd, struct sockaddr_in *client, socklen_t len, uint16_t block)
{
    tftp_packet ack;
    memset(&ack, 0, sizeof(ack));

    ack.opcode = htons(OP_ACK);
    ack.ack.block = htons(block);

    sendto(sockfd, &ack, 4, 0, (struct sockaddr *)client, len);
}