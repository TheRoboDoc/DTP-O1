#include "dtp_common.h"

int dtp_run_client(sock_t sockfd, const dtp_config_t* cfg) 
{
    uint8_t sendBuf[256];
    uint8_t flags = FLAG_PRIO;

    size_t len = buildFrame(sendBuf, cfg->originId, cfg->destId, CODE_INITIATE, flags, NULL, 0);

    struct sockaddr_in addr = { 0 };

    addr.sin_family = AF_INET;
    addr.sin_port = htons(cfg->port);

    inet_pton(AF_INET, cfg->remoteHost, &addr.sin_addr);

    if (sendto(sockfd, sendBuf, (int)len, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    #ifdef _WIN32
        fprintf(stderr, "sendto() failed: %d\n", WSAGetLastError());
    #else
        perror("sendto");
    #endif
        return 1;
    }

    uint8_t recvBuf[256];

    struct sockaddr_in from;

    socklen_t fromLen = sizeof(from);

    SSIZE_T n = recvfrom(sockfd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&from, &fromLen);

    if (n <= 0) 
    {
        fprintf(stderr, "No response or recv error\n");

        return 1;
    }

    if (n < 13) return 1;

    uint16_t hdr = ntohs(*(uint16_t*)recvBuf);
    uint16_t ftr = ntohs(*(uint16_t*)(recvBuf + n - 2));

    if (hdr != HEADER_MAGIC || ftr != FOOTER_MAGIC) return 1;

    uint16_t crc = ntohs(*(uint16_t*)(recvBuf + n - 4));
    uint16_t calc = compute_crc16(recvBuf + 2, n - 4 - 2);

    if (crc != calc) return 1;

    uint8_t code = recvBuf[7];

    if (code == CODE_ACK) 
    {
        printf("Client: Handshake ACK received.\n");
    }
    else if (code == CODE_ERROR) 
    {
        fprintf(stderr, "Client: Received ERROR_REPORT.\n");
    }
    else 
    {
        fprintf(stderr, "Client: Unexpected code 0x%02X.\n", code);
    }

    return 0;
}
