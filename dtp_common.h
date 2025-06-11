#pragma once

#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>

    #pragma comment(lib, "Ws2_32.lib")

    typedef SOCKET sock_t;

    #define SOCK_INVALID INVALID_SOCKET
    #define SOCK_CLOSE closesocket
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <unistd.h>

    typedef int sock_t;

    #define SOCK_INVALID -1
    #define SOCK_CLOSE close
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER_MAGIC 0xAA55
#define FOOTER_MAGIC 0x55AA
#define DEFAULT_PORT 4141
#define DEFAULT_ORIGIN 0x1000
#define DEFAULT_DEST 0x2000
#define HANDSHAKE_TIMEOUT_MS 800

#define CODE_INITIATE 0x01
#define CODE_ACK      0x02
#define CODE_ERROR    0x08
#define CODE_SEVER    0x09

#define FLAG_ACK  0x01
#define FLAG_RST  0x02
#define FLAG_PRIO 0x04
#define FLAG_ERR  0x08

typedef struct 
{
    bool isClient;

    char remoteHost[256];

    unsigned short port;

    uint16_t originId;
    uint16_t destId;

    bool valid;
} dtp_config_t;

uint16_t compute_crc16(const uint8_t* data, size_t len);

dtp_config_t dtp_parse_args(int argc, char* argv[]);

sock_t dtp_socket_setup(const dtp_config_t* cfg);

int dtp_run_client(sock_t sockfd, const dtp_config_t* cfg);
int dtp_run_server(sock_t sockfd, const dtp_config_t* cfg);

size_t buildFrame(uint8_t* buf, uint16_t origin, uint16_t dest,
    uint8_t code, uint8_t flags,
    const uint8_t* payload, uint16_t payloadLen);
