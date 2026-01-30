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

#define TFTP_PORT 6000
#define DATA_SIZE 512
#define PACKET_SIZE 516
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

typedef struct
{
    int sock_fd;
    struct sockaddr_in server_addr;
    socklen_t addr_len;
    char mode[10]; // octet or netascii
} tftp_client;

#pragma pack(pop)

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

/* FUNCTIONS */

// help
void help(void);

// connect
void connect_server(tftp_client *client);
result validate_ip(const char *ip);
void init_client(tftp_client *client);
result send_connect_request(tftp_client *client, char *ip);
result receive_connect_ack(tftp_client *client);
void print_server_info(tftp_client *client);

// put file to server
result send_file_to_server(tftp_client *client);
int check_file_exists(const char *filename);
void set_socket_timeout(int sockfd);
int build_wrq(tftp_packet *pkt, const char *filename, const char *mode);
result recv_ack_with_retry(tftp_client *client, uint16_t block);
int netascii_read(int fd, char *buf);
result send_data_with_retry(tftp_client *client, tftp_packet *pkt, int size, uint16_t block);

// get file from server
result get_file_from_server(tftp_client *client);
int build_rrq(tftp_packet *pkt, const char *filename, const char *mode);
void generate_unique_filename(const char *original, char *newname, size_t size);
void netascii_write(int fd, char *buf, int len);

// change mode
void change_mode(tftp_client *client);

// end
void end(tftp_client *client);

#endif