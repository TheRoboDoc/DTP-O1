#include "dtp_common.h"

static void printUsage(const char* progName) 
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s server [port] [originID]\n", progName);
    fprintf(stderr, "  %s client [host] [port] [originID] [destID]\n", progName);
}

dtp_config_t dtp_parse_args(int argc, char* argv[]) 
{
    dtp_config_t config = 
    {
        .isClient = false,
        .port = DEFAULT_PORT,
        .originId = DEFAULT_ORIGIN,
        .destId = DEFAULT_DEST,
        .valid = true
    };

    strncpy(config.remoteHost, "127.0.0.1", sizeof(config.remoteHost) - 1);

    if (argc <= 1) 
    {
        config.valid = false;

        return config;
    }

    if (strcmp(argv[1], "server") == 0) 
    {
        config.isClient = false;

        if (argc > 2) config.port = (unsigned short)atoi(argv[2]);

        if (argc > 3) config.originId = (uint16_t)strtol(argv[3], NULL, 0);

        return config;
    }

    if (strcmp(argv[1], "client") == 0) 
    {
        config.isClient = true;

        if (argc > 2) strncpy(config.remoteHost, argv[2], sizeof(config.remoteHost) - 1);
        if (argc > 3) config.port = (unsigned short)atoi(argv[3]);
        if (argc > 4) config.originId = (uint16_t)strtol(argv[4], NULL, 0);
        if (argc > 5) config.destId = (uint16_t)strtol(argv[5], NULL, 0);

        return config;
    }

    printUsage(argv[0]);

    config.valid = false;

    return config;
}

sock_t dtp_socket_setup(const dtp_config_t* cfg) 
{
#ifdef _WIN32
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        fprintf(stderr, "WSAStartup failed.\n");

        return SOCK_INVALID;
    }
#endif

    sock_t sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == SOCK_INVALID) 
    {
    #ifdef _WIN32
        fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
    #else
        perror("socket");
    #endif
        return SOCK_INVALID;
    }

    if (!cfg->isClient) 
    {
        struct sockaddr_in addr = { 0 };

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(cfg->port);

        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) 
        {
        #ifdef _WIN32
            fprintf(stderr, "bind() failed: %d\n", WSAGetLastError());
        #else
            perror("bind");
        #endif
            SOCK_CLOSE(sockfd);
        #ifdef _WIN32
            WSACleanup();
        #endif
            return SOCK_INVALID;
        }
    }
    else 
    {
    #ifdef _WIN32
        DWORD timeout = HANDSHAKE_TIMEOUT_MS;

        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    #else
        struct timeval tv = { .tv_sec = 0, .tv_usec = HANDSHAKE_TIMEOUT_MS * 1000 };
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    #endif
    }

    return sockfd;
}
