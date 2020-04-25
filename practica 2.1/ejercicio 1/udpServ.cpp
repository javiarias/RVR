#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char **argv)
{
    int BUFFSIZE = 500;

    struct addrinfo hints;
    struct addrinfo * res;

    char buffer[BUFFSIZE];
    
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

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if(bind(sd, (struct sockaddr *) res->ai_addr, res->ai_addrlen) != 0)
    {
        std::cerr << "bind: " << std::endl;
        return -1;
    }

    while (1) 
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr* cliente = res->ai_addr;
        socklen_t cliente_len = res->ai_addrlen;

        int bytes = recvfrom(sd, buffer, (BUFFSIZE - 1) * sizeof(char), 0, cliente, &cliente_len);  
        
        getnameinfo(cliente, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);

        buffer[bytes - 1] = '\0';
        
        printf("Conexión desde Host:%s Puerto:%s\n", host, serv);
        
        if(strcmp(buffer, "t") == 0)
        {
            char out[100];
            time_t rawtime;
            struct tm * timeinfo;

            time (&rawtime);
            timeinfo = localtime (&rawtime);


            strftime(out, 100, "%r \n", timeinfo);

            sendto(sd, out, strlen(out), 0, cliente, cliente_len);
        }
        else if(strcmp(buffer, "d") == 0)
        {
            char out[100];
            time_t rawtime;
            struct tm * timeinfo;

            time (&rawtime);
            timeinfo = localtime (&rawtime);

            strftime(out, 100, "%D \n", timeinfo);

            sendto(sd, out, strlen(out), 0, cliente, cliente_len);
        }
        else if (strcmp(buffer, "q") == 0)
        {
            printf("Closing...\n");
            return 0;
        }
        else
        {
            char out[] = "Error: no such command \n";
            
            sendto(sd, out, strlen(out), 0, cliente, cliente_len);
        }
    }

    freeaddrinfo(res);
    close(sd);
    
    return 0;
}