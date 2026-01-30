#include "main.h"

result send_file_to_server(tftp_client *client)
{
    char filename[256];
    tftp_packet pkt;
    tftp_packet resp;
    int fd, data_len, pkt_len;
    uint16_t block = 1;

    /* Check connection */
    if (client->sock_fd < 0)
    {
        printf("Not connected to server\n");
        return FAILURE;
    }

    printf("Enter the File_name : ");
    scanf("%s", filename);

    /* Check file exists on client */
    if (!check_file_exists(filename))
    {
        printf(GRAY"INFO : File not found on client"COLOREND"\n\n");
        return FAILURE;
    }

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return FAILURE;
    }

    /* Set timeout for retransmission */
    set_socket_timeout(client->sock_fd);

     client->server_addr.sin_port = htons(TFTP_PORT);  //always port need to be tftp port

    /* SEND WRQ */
    pkt_len = build_wrq(&pkt, filename, client->mode);

    sendto(client->sock_fd, &pkt, pkt_len, 0, (struct sockaddr *)&client->server_addr, client->addr_len);

    /* HANDLE WRQ RESPONSE */
    while (1)
    {
        int ret = recvfrom(client->sock_fd, &resp, sizeof(resp), 0, (struct sockaddr *)&client->server_addr, &client->addr_len);

        if (ret < 0)
        {
            perror("recvfrom");
            close(fd);
            return FAILURE;
        }

        /* Server accepted WRQ */
        if (ntohs(resp.opcode) == OP_ACK && ntohs(resp.ack.block) == 0)
        {
            break;
        }

        /* File already exists on server */
        if (ntohs(resp.opcode) == OP_ERROR && ntohs(resp.error.error_code) == 6)
        {
            char choice;
            printf("%s. Overwrite? (y/n): ",resp.error.error_msg);
            scanf(" %c", &choice);

            if (choice != 'y')
            {
                printf(RED"Info : Sending aborted by user"COLOREND"\n\n");
                close(fd);
                return FAILURE;
            }

            /* resend WRQ if overwrite allowed */

            printf("INFO : Overwriting file on server...\n");
            sendto(client->sock_fd, &pkt, pkt_len, 0, (struct sockaddr *)&client->server_addr, client->addr_len);
        }
    }

    /* ---------- SEND DATA BLOCKS ---------- */
    while (1)
    {
        pkt.opcode = htons(OP_DATA);
        pkt.data_pkt.block = htons(block);

        if (strcmp(client->mode, "octet") == 0)
            data_len = read(fd, pkt.data_pkt.data, DATA_SIZE);
        else
            data_len = netascii_read(fd, pkt.data_pkt.data);

        if (data_len < 0)
        {
            perror("read");
            close(fd);
            return FAILURE;
        }

        if (send_data_with_retry(client, &pkt, data_len + 4, block) != SUCCESS)
        {
            printf("Transmission failed\n");
            close(fd);
            return FAILURE;
        }

        if (data_len < DATA_SIZE)
            break;

        block++;
    }

    close(fd);
    printf(BLUE"File sent successfully"COLOREND"\n\n");
    return SUCCESS;
}

int check_file_exists(const char *filename)
{
    return access(filename, F_OK) == 0;
}

void set_socket_timeout(int sockfd)
{
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

int build_wrq(tftp_packet *pkt, const char *filename, const char *mode)
{
    memset(pkt, 0, sizeof(*pkt));

    pkt->opcode = htons(OP_WRQ);
    strcpy(pkt->request.filename, filename);
    strcpy(pkt->request.mode, mode);

    return 2 + strlen(filename) + 1 + strlen(mode) + 1;
}

result recv_ack_with_retry(tftp_client *client, uint16_t block)
{
    tftp_packet pkt;
    int retry = 0;

    while (retry < MAX_RETRY)
    {
        int ret = recvfrom(client->sock_fd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&client->server_addr, &client->addr_len);

        if (ret > 0 && ntohs(pkt.opcode) == OP_ACK && ntohs(pkt.ack.block) == block)
        {
            return SUCCESS;
        }

        retry++;
        printf("Retry ACK %d (%d/%d)\n", block, retry, MAX_RETRY);
    }

    return FAILURE;
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

result send_data_with_retry(tftp_client *client, tftp_packet *pkt, int size, uint16_t block)
{
    int retry = 0;

    while (retry < MAX_RETRY)
    {
        sendto(client->sock_fd, pkt, size, 0, (struct sockaddr *)&client->server_addr, client->addr_len);

        if (recv_ack_with_retry(client, block) == SUCCESS)
            return SUCCESS;

        retry++;
        printf("Retransmitting block %d (%d/%d)\n", block, retry, MAX_RETRY);
    }

    return FAILURE;
}