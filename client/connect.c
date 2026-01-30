#include "main.h"

void init_client(tftp_client *client)
{
    client->sock_fd = -1;
    client->addr_len = sizeof(client->server_addr);
    strcpy(client->mode, "octet");
}

void connect_server(tftp_client *client)
{
    char ip[20];
    printf("Enter the Server IP : ");
    scanf("%s", ip);

    if (validate_ip(ip) == SUCCESS)
    {
        if (send_connect_request(client, ip) == SUCCESS && receive_connect_ack(client) == SUCCESS)
        {
            print_server_info(client);
        }
        else
        {
            printf(RED "ERROR : Connection Failed" COLOREND "\n");
        }
    }
    else
    {
        printf(RED "ERROR : Invalid IP Address Format" COLOREND "\n");
    }
    
}

result validate_ip(const char *ip)
{
    struct sockaddr_in sa;

    if (inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1)
    {
        return SUCCESS;
    }
    else
        return FAILURE;
}

result send_connect_request(tftp_client *client, char *ip)
{
    tftp_packet pkt;
    int pkt_len;

    /*create socket*/
    client->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (client->sock_fd < 0)
    {
        perror("Socket");
        return FAILURE;
    }

    /*server address*/
    memset(&client->server_addr, 0, sizeof(client->server_addr));
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(TFTP_PORT);
    client->server_addr.sin_addr.s_addr = inet_addr(ip);

    /* Build dummy RRQ*/
    pkt.opcode = htons(OP_RRQ);
    strcpy(pkt.request.filename, "connect");
    strcpy(pkt.request.mode, client->mode);

    pkt_len = 2 + strlen("connect") + 1 + strlen(client->mode) + 1;

    sendto(client->sock_fd, &pkt, pkt_len, 0, (struct sockaddr *)&client->server_addr, client->addr_len);

    return SUCCESS;
}

result receive_connect_ack(tftp_client *client)
{
    tftp_packet pkt;

    int bytes = recvfrom(client->sock_fd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&client->server_addr, &client->addr_len);

    if (bytes < 0)
    {
        perror("recvfrom");
        return FAILURE;
    }

    tftp_opcode check = ntohs(pkt.opcode);
    if (check == OP_ACK)
    {
        return SUCCESS;
    }
    else if (check == OP_ERROR)
    {
        printf(RED "Server error %d: %s\n" COLOREND, ntohs(pkt.error.error_code), pkt.error.error_msg);
    }

    printf(RED "Invalid response from server\n" COLOREND);
    return FAILURE;
}

void print_server_info(tftp_client *client)
{
    char ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &client->server_addr.sin_addr, ip, sizeof(ip));

    printf(GREEN "\n\t\t\t Connected Successfully!\n" COLOREND);
    printf("Server IP   : %s\n", ip);
    printf("Server Port : %d\n", ntohs(client->server_addr.sin_port));
    printf("Mode        : %s\n\n", client->mode);
}