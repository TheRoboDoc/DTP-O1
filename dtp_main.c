#include "dtp_common.h"

int main(int argc, char* argv[]) 
{
    dtp_config_t config = dtp_parse_args(argc, argv);

    if (!config.valid) return 1;

    sock_t sockfd = dtp_socket_setup(&config);

    if (sockfd == SOCK_INVALID) return 1;

    int status = config.isClient
        ? dtp_run_client(sockfd, &config)
        : dtp_run_server(sockfd, &config);

    SOCK_CLOSE(sockfd);

#ifdef _WIN32
    WSACleanup();
#endif
    return status;
}
