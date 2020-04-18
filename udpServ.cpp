#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#include <iostream>

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * result;
    char buffer[500];
    
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

    bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);

    while (1) 
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr cliente;
        socklen_t cliente_len;

        int bytes = recvfrom(sd, buffer, 500, 0, (struct sockaddr *) &cliente, &cliente_len);  
        
        getnameinfo((struct sockaddr *) &cliente, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);

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

            sendto(sd, out, strlen(out), 0, (struct sockaddr *) &cliente, cliente_len);
        }
        else if(strcmp(buffer, "d") == 0)
        {
            char out[100];
            time_t rawtime;
            struct tm * timeinfo;

            time (&rawtime);
            timeinfo = localtime (&rawtime);

            strftime(out, 100, "%D \n", timeinfo);

            sendto(sd, out, strlen(out), 0, (struct sockaddr *) &cliente, cliente_len);
        }
        else if (strcmp(buffer, "q") == 0)
        {
            printf("Closing...\n");
            return 0;
        }
        else
        {
            char out[] = "Error: no such command \n";
            
            sendto(sd, out, strlen(out), 0, (struct sockaddr *) &cliente, cliente_len);
        }
    }
    
    return 0;
}