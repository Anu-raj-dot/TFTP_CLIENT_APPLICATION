#include "main.h"

void set_timeout(int sockfd)
{
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

void send_error(int sockfd,  struct sockaddr_in *client, socklen_t len, uint16_t code, const char *msg)
{
    tftp_packet err;
    memset(&err, 0, sizeof(err));

    err.opcode = htons(OP_ERROR);
    err.error.error_code = htons(code);
    strcpy(err.error.error_msg, msg);

    sendto(sockfd, &err, 4 + strlen(msg) + 1, 0, (struct sockaddr *)client, len);
}