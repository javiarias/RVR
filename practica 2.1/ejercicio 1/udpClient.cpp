#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <cstdio>
#include <unistd.h>

#include <iostream>

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * res;
    char buffer[500];
    char in[500];

    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family   = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if ( rc != 0 )
    {
        std::cerr << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);

    while (1)
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr * servidor = res->ai_addr;
        socklen_t servidor_len = res->ai_addrlen;

        fgets(buffer, 500, stdin);

        sendto(sd, buffer, strlen(buffer), 0, servidor, servidor_len);  

        int bytes = recvfrom(sd, in, 500, 0, servidor, &servidor_len);

        in[bytes] = '\0';

        printf(in);
    }

    freeaddrinfo(res);
    close(sd);

    return 0;
}