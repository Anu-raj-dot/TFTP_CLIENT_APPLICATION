#include "main.h"

void handle_rrq(tftp_packet *req, tftp_server *server)
{
    int port = server->transfer_port++;
    pid_t pid = fork();

    if (pid == 0) // CHILD
    {
        int data_sock;
        struct sockaddr_in data_addr;

        data_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (data_sock < 0)
        {
            perror("socket");
            exit(1);
        }

        memset(&data_addr, 0, sizeof(data_addr));
        data_addr.sin_family = AF_INET;
        data_addr.sin_addr.s_addr = inet_addr(TFTP_SERVER_IP);
        data_addr.sin_port = htons(port);

        if (bind(data_sock, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0)
        {
            perror("bind");
            close(data_sock);
            exit(1);
        }

        printf(GRAY BOLD "INFO : [SERVER] GET file processing on port %d" COLOREND "\n", port);

        handle_get_file(data_sock, req, &server->client_addr, server->client_len, req->request.mode);

        close(data_sock);
        exit(0);
    }
}

void handle_get_file(int sockfd, tftp_packet *req, struct sockaddr_in *client, socklen_t client_len, const char *mode)
{
    int fd;
    tftp_packet pkt;
    uint16_t block = 1;
    int data_len;

    printf(BLUE "INFO : [SERVER] GET request for file: " COLOREND "%s\n", req->request.filename);

    /* File not found */
    if (access(req->request.filename, F_OK) != 0)
    {
        send_error(sockfd, client, client_len, 1, "File not present in server");
        return;
    }

    fd = open(req->request.filename, O_RDONLY);
    if (fd < 0)
    {
        send_error(sockfd, client, client_len, 2, "Access violation while opening file");
        return;
    }

    set_timeout(sockfd);

    while (1)
    {
        pkt.opcode = htons(OP_DATA);
        pkt.data_pkt.block = htons(block);

        if (strcmp(mode, "octet") == 0)
            data_len = read(fd, pkt.data_pkt.data, DATA_SIZE);
        else
            data_len = netascii_read(fd, pkt.data_pkt.data);

        if (data_len < 0)
        {
            perror("read");
            close(fd);
            return;
        }

        if (send_data_with_retry(sockfd, client, client_len, &pkt, data_len + 4, block) != SUCCESS)
        {
            printf(RED "ERROR : [SERVER] Transfer failed" COLOREND "\n");
            close(fd);
            return;
        }

        if (data_len < DATA_SIZE)
            break;

        block++;
    }

    close(fd);
    printf(GREEN "INFO : [SERVER] File sent successfully" COLOREND "\n");
}

int netascii_read(int fd, char *buf)
{
    int i = 0;
    char ch;

    while (i < DATA_SIZE && read(fd, &ch, 1) == 1)
    {
        if (ch == '\n')
        {
            buf[i++] = '\r';
            if (i < DATA_SIZE)
                buf[i++] = '\n';
        }
        else
        {
            buf[i++] = ch;
        }
    }
    return i;
}

result send_data_with_retry(int sockfd, struct sockaddr_in *client, socklen_t client_len, tftp_packet *pkt, int size, uint16_t block)
{
    tftp_packet ack;
    int retry = 0;

    while (retry < MAX_RETRY)
    {
        sendto(sockfd, pkt, size, 0, (struct sockaddr *)client, client_len);

        int ret = recvfrom(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)client, &client_len);

        if (ret > 0 && ntohs(ack.opcode) == OP_ACK && ntohs(ack.ack.block) == block)
        {
            return SUCCESS;
        }

        retry++;

        printf(RED "INFO " COLOREND ":" BLUE " [SERVER] Retransmitting block" COLOREND " %d (%d/%d)\n", block, retry, MAX_RETRY);
    }

    return FAILURE;
}
