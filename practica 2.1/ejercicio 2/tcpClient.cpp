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
    int BUFFER_SIZE = 500;

    struct addrinfo hints;
    struct addrinfo * res;
    char buffer[BUFFER_SIZE];
    char in[BUFFER_SIZE];

    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family   = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if ( rc != 0 )
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if( connect(sd, res->ai_addr, res->ai_addrlen) != 0 )
    {
        std::cerr << "uh oh, connect failed " << std::endl;
        return -1;
    }

    while (1)
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr * servidor = res->ai_addr;
        socklen_t servidor_len = res->ai_addrlen;

        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0';

        send(sd, (void*) buffer, strlen(buffer), 0);  

        ssize_t bytes = recv(sd, (void *) in, sizeof(char)*(BUFFER_SIZE - 1), 0);

        in[bytes] = '\0';

        printf("echo: ");
        printf(in);
        printf("\n");

        if(strcmp(buffer, "Q") == 0 || strcmp(buffer, "q") == 0)
            return 0;

        bzero(buffer, sizeof(buffer));
        bzero(in, sizeof(in));
    }

    freeaddrinfo(res);
    close(sd);

    return 0;
}