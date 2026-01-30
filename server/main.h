#ifndef MAIN_H
#define MAIN_H

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT "\033[0m"
#define BOLD "\033[1m"

#define GRAY "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#define REDBACKGROUND "\033[41m"
#define GREENBACKGROUND "\033[42m"
#define YELLOWBACKGROUND "\033[43m"
#define BLUEBACKGROUND "\033[44m"
#define PINKBACKGROUND "\033[45m"
#define CYANBACKGROUND "\033[46m"
#define GRAYBACKGROUND "\033[47m"

#define COLOREND "\033[0m"

#define TFTP_SERVER_IP "127.0.0.1"
#define TFTP_SERVER_PORT 6000
#define START_CHILD_PORT 20000
#define DATA_SIZE 512
#define MAX_CLIENTS 10
#define TIMEOUT_SEC 5
#define MAX_RETRY 5

typedef enum
{
    FAILURE = -1,
    SUCCESS
} result;

typedef enum
{
    OP_RRQ = 1,
    OP_WRQ,
    OP_DATA,
    OP_ACK,
    OP_ERROR
} tftp_opcode;

#pragma pack(push, 1)

typedef struct
{

    uint16_t opcode;

    union
    {
        /* RRQ/WRQ */
        struct
        {
            char filename[256];
            char mode[10]; // "octet" or "netascii"
        } request;

        /* DATA */
        struct
        {
            uint16_t block;
            char data[DATA_SIZE];
        } data_pkt;

        /* ACK */
        struct
        {
            uint16_t block;
        } ack;

        /* ERROR */
        struct
        {
            uint16_t error_code;
            char error_msg[128];
        } error;
    };

} tftp_packet;

typedef struct // server context
{
    int sock_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int transfer_port;
} tftp_server;

/* ---------------- FUNCTION DECLARATIONS ---------------- */

//common
void set_timeout(int sockfd);
void send_error(int sockfd, struct sockaddr_in *client, socklen_t len, uint16_t code, const char *msg);

// to connect
result init_server(tftp_server *server);
void handle_new_client(tftp_server *server);
int bind_child_socket(int port);
void child_send_ack(int sockfd, tftp_server *server);

// to recieve file
void handle_wrq(tftp_packet *req, tftp_server *server);
void handle_put_file(int sockfd, tftp_packet *req, struct sockaddr_in *client, socklen_t client_len);
void send_ack(int sockfd, struct sockaddr_in *client, socklen_t len, uint16_t block);

//to send file
void handle_rrq(tftp_packet *req, tftp_server *server);
void handle_get_file(int sockfd, tftp_packet *req, struct sockaddr_in *client, socklen_t client_len, const char *mode);
result send_data_with_retry(int sockfd, struct sockaddr_in *client, socklen_t client_len, tftp_packet *pkt, int size, uint16_t block);
int netascii_read(int fd, char *buf);


#endif