#include "main.h"

void end(tftp_client *client)
{
    tftp_packet pkt;
    int pkt_len;

    /* Check connection */
    if (client->sock_fd < 0)
    {
        return;
    }

    client->server_addr.sin_port = htons(TFTP_PORT); // always port need to be tftp port

    /* Build END RRQ*/
    pkt.opcode = htons(OP_RRQ);
    strcpy(pkt.request.filename, "end");
    strcpy(pkt.request.mode, client->mode);

    pkt_len = 2 + strlen("connect") + 1 + strlen(client->mode) + 1;

    sendto(client->sock_fd, &pkt, pkt_len, 0, (struct sockaddr *)&client->server_addr, client->addr_len);
}