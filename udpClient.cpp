#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <cstdio>

#include <iostream>

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * result;
    char buffer[500];
    char in[500];
    
    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family   = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    if ( rc != 0 )
    {
        std::cerr << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    while (1)
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr servidor;
        socklen_t servidor_len;

        printf("Input message >");

        fgets(buffer, 500, stdin);

        printf("Sending message...");

        sendto(sd, buffer, strlen(buffer), 0, (struct sockaddr *) &servidor, sizeof(servidor));  
        
        printf("Message sent!");

        int bytes = recvfrom(sd, in, 500, 0, (struct sockaddr *) &servidor, &servidor_len);

        printf("Message received!");
        printf(in);
    }
    
    return 0;
}