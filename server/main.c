#include "main.h"

int main()
{
    tftp_server server;
    tftp_packet pkt;

    if (init_server(&server) != SUCCESS)
    {
        printf(RED"Info : Server initialization failed"COLOREND"\n");
        exit(1);
    }

    printf(GREEN"TFTP Server running on port %d..."COLOREND"\n", TFTP_SERVER_PORT);

    while (1)
    {
        int bytes = recvfrom(server.sock_fd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&server.client_addr, &server.client_len);
        if (bytes < 0)
        {
            perror("recvfrom");
            continue;
        }

        tftp_opcode op_code = ntohs(pkt.opcode);
        if (op_code == OP_RRQ)
        {
            if(!strcmp(pkt.request.filename, "connect"))
            {
                handle_new_client(&server);
            }
            else if(!strcmp(pkt.request.filename, "end"))
            {
                printf(RED"\t\t\tINFO : SERVER IS SHUTTING DOWN"COLOREND"\n");
                exit(0);
            }
            else
            {
                handle_rrq(&pkt, &server);
            }
            
 
        }
        else if(op_code == OP_WRQ)
        {
            handle_wrq(&pkt, &server);
        }
    }

    close(server.sock_fd);
    return 0;
}