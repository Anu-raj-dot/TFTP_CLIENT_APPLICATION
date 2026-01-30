#include "main.h"

result get_file_from_server(tftp_client *client)
{
    tftp_packet pkt, resp, ack;
    char filename[100];
    char local_file[100];
    int fd;
    uint16_t expected_block = 1;
    int pkt_len, ret;

    if (client->sock_fd < 0)
    {
        printf(RED "INFO : Not connected to server" COLOREND "\n\n");
        return FAILURE;
    }

    printf(BLUE BOLD "Enter file name to download: " COLOREND);
    scanf("%s", filename);

    /* Always send RRQ to well-known TFTP port */
    client->server_addr.sin_port = htons(TFTP_PORT);

    pkt_len = build_rrq(&pkt, filename, client->mode);

    sendto(client->sock_fd, &pkt, pkt_len, 0, (struct sockaddr *)&client->server_addr, client->addr_len);

    set_socket_timeout(client->sock_fd);

    /* Wait for first response */
    ret = recvfrom(client->sock_fd, &resp, sizeof(resp), 0, (struct sockaddr *)&client->server_addr, &client->addr_len);

    if (ret < 0)
    {
        perror("recvfrom");
        return FAILURE;
    }

    /* File not present on server */
    if (ntohs(resp.opcode) == OP_ERROR && ntohs(resp.error.error_code) == 1)
    {
        printf(RED BOLD "INFO : %s" COLOREND "\n\n",resp.error.error_msg);
        return FAILURE;
    }
    else if(ntohs(resp.opcode) == OP_ERROR && ntohs(resp.error.error_code) == 2)
    {
        printf(RED BOLD "ERROR :[Server] %s" COLOREND "\n\n",resp.error.error_msg);
        return FAILURE;
    }

    /* Expect DATA block 1 */
    if (ntohs(resp.opcode) != OP_DATA || ntohs(resp.data_pkt.block) != 1)
    {
        printf(RED "ERROR : Invalid server response" COLOREND "\n");
        return FAILURE;
    }

    /* Create unique local filename */
    generate_unique_filename(filename, local_file, sizeof(local_file));
    printf(BLUE BOLD "\nSaving file as: %s" COLOREND "\n\n", local_file);

    fd = open(local_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
    {
        perror("open");
        return FAILURE;
    }

    /* Receive DATA blocks */
    while (1)
    {
        int data_len = ret - 4; // 4 is (2 bytes opcode + 2 bytes block)

        if (strcmp(client->mode, "octet") == 0)
            write(fd, resp.data_pkt.data, data_len);
        else
            netascii_write(fd, resp.data_pkt.data, data_len);

        /* Send ACK */
        ack.opcode = htons(OP_ACK);
        ack.ack.block = htons(expected_block);

        sendto(client->sock_fd, &ack, 4, 0, (struct sockaddr *)&client->server_addr, client->addr_len);

        if (data_len < DATA_SIZE)
            break; // last block/packet

        expected_block++;

        ret = recvfrom(client->sock_fd, &resp, sizeof(resp), 0, (struct sockaddr *)&client->server_addr, &client->addr_len);

        if (ret < 0)
        {
            perror("recvfrom");
            close(fd);
            return FAILURE;
        }

        if (ntohs(resp.opcode) != OP_DATA || ntohs(resp.data_pkt.block) != expected_block)
            continue;
    }

    close(fd);
    printf(GREEN BOLD "\nFile downloaded successfully" COLOREND "\n\n");
    return SUCCESS;
}

int build_rrq(tftp_packet *pkt, const char *filename, const char *mode)
{
    memset(pkt, 0, sizeof(*pkt));
    pkt->opcode = htons(OP_RRQ);
    strcpy(pkt->request.filename, filename);
    strcpy(pkt->request.mode, mode);

    return 2 + strlen(filename) + 1 + strlen(mode) + 1;
}

void generate_unique_filename(const char *original, char *newname, size_t size)
{
    char base[256], ext[64];
    int count = 0;

    char *dot = strrchr(original, '.');
    if (dot)
    {
        strncpy(base, original, dot - original);
        base[dot - original] = '\0';
        strcpy(ext, dot);
    }
    else
    {
        strcpy(base, original);
        ext[0] = '\0';
    }

    snprintf(newname, size, "%s%s", base, ext);
    if (access(newname, F_OK) != 0)
        return;

    do
    {
        count++;
        snprintf(newname, size, "%s(%d)%s", base, count, ext);

    } while (access(newname, F_OK) == 0);
}

void netascii_write(int fd, char *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (buf[i] == '\r' && i + 1 < len && buf[i + 1] == '\n')
        {
            write(fd, "\n", 1);
            i++;
        }
        else
        {
            write(fd, &buf[i], 1);
        }
    }
}
