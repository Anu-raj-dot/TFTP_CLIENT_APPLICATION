#include "main.h"

void help(void)
{
    fprintf(stderr, "(1) CONNECT : Used to connect to a server\n");
    fprintf(stderr, "(2) PUT FILE : Used to upload a file to a server from client\n");
    fprintf(stderr, "(3) GET FILE : Used to download a file from a server to client\n");
    fprintf(stderr, "(4) MODE : Used to change mode between netascii and octet\n");
    fprintf(stderr, "(5) EXIT : Used to exit from the program\n\n");
}



    // char ip[INET_ADDRSTRLEN];                                   //test

    // inet_ntop(AF_INET,
    //           &client->server_addr.sin_addr,
    //           ip,
    //           sizeof(ip));

    // printf("\n========== DEBUG () ==========\n");                    //test
    // printf("sock_fd_3        : %d\n", client->sock_fd);
    // printf("server IP      : %s\n", ip);
    // printf("server Port    : %d\n", ntohs(client->server_addr.sin_port));
    // printf("addr_len       : %d\n", client->addr_len);
    // printf("mode           : %s\n", client->mode);
    // printf("================================\n\n");