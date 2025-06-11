#include "dtp_common.h"

int dtp_run_server(sock_t sockfd, const dtp_config_t* cfg) 
{
    uint8_t buf[256];

    struct sockaddr_in peer;

    socklen_t peerLen = sizeof(peer);

    while (true) 
    {
        int n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&peer, &peerLen);

        if (n < 0) 
        {
        #ifdef _WIN32
            fprintf(stderr, "recvfrom() failed: %d\n", WSAGetLastError());
        #else
            perror("recvfrom");
        #endif
            return 1;
        }

        if (n < 13) continue;

        uint16_t hdr = ntohs(*(uint16_t*)buf);
        uint16_t ftr = ntohs(*(uint16_t*)(buf + n - 2));

        if (hdr != HEADER_MAGIC || ftr != FOOTER_MAGIC) continue;

        uint16_t crc = ntohs(*(uint16_t*)(buf + n - 4));
        uint16_t calc = compute_crc16(buf + 2, n - 4 - 2);

        if (crc != calc) continue;

        uint8_t code = buf[7];

        uint16_t origin = ntohs(*(uint16_t*)(buf + 3));

        if (code == CODE_INITIATE) 
        {
            printf("Server: INITIATE received from 0x%04X\n", origin);

            uint16_t token = 0xB00B;

            uint8_t payload[2];

            uint16_t net = htons(token);

            memcpy(payload, &net, 2);

            size_t len = buildFrame(buf, cfg->originId, origin, CODE_ACK, FLAG_ACK, payload, 2);

            sendto(sockfd, buf, (int)len, 0, (struct sockaddr*)&peer, peerLen);
        }
        else if (code == CODE_SEVER) 
        {
            printf("Server: SEVER received.\n");
        }
        else 
        {
            printf("Server: Unknown code 0x%02X\n", code);

            size_t len = buildFrame(buf, cfg->originId, origin, CODE_ERROR, FLAG_ERR, NULL, 0);

            sendto(sockfd, buf, (int)len, 0, (struct sockaddr*)&peer, peerLen);
        }
    }
}
