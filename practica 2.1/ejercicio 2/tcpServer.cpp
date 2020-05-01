 #include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

/*
./tcp 0.0.0.0 7777
*/

int main(int argc, char **argv)
{
    int BUFFER_SIZE = 500;
    int BACKLOG = 20;
    struct addrinfo hints;
    struct addrinfo * res;

    // ---------------------------------------------------------------------- //
    // INICIALIZACIÓN SOCKET & BIND //
    // ---------------------------------------------------------------------- //

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if ( rc != 0 )
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    // res contiene la representación como sockaddr de dirección + puerto

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if ( bind(sd, res->ai_addr, res->ai_addrlen) != 0 )
    {
        std::cerr << "bind error" << std::endl;
        return -1;
    }

    freeaddrinfo(res);

// ---------------------------------------------------------------------- //
// PUBLICAR EL SERVIDOR (LISTEN) //
// ---------------------------------------------------------------------- //
    //BACKLOG es el tamaño del backlog de conexiones
    listen(sd, BACKLOG);

// ---------------------------------------------------------------------- //
// GESTION DE LAS CONEXIONES AL SERVIDOR //
// ---------------------------------------------------------------------- //
    
    struct sockaddr* client_addr = res->ai_addr;
    socklen_t client_len = res->ai_addrlen;

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int sd_client = accept(sd, client_addr, &client_len);

    getnameinfo(client_addr, client_len, host, NI_MAXHOST, service,
    NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout << "CONEXION DESDE IP: " << host << " PUERTO: " << service << std::endl;

// ---------------------------------------------------------------------- //
// GESTION DE LA CONEXION CLIENTE //
// ---------------------------------------------------------------------- //
    while(true)
    {
        char buffer[BUFFER_SIZE];

        ssize_t bytes = recv(sd_client, (void *) buffer, sizeof(char)*(BUFFER_SIZE - 1), 0);

        //if (bytes <= 0)
            //return 0;

        buffer[bytes] = '\0';

        std::cout << "MENSAJE: " << buffer << std::endl;

        send(sd_client, (void*) buffer, bytes, 0);

        if(strcmp(buffer, "Q") == 0 || strcmp(buffer, "q") == 0)
            return 0;
        
        bzero(buffer, sizeof(buffer));
    }

    freeaddrinfo(res);
    close(sd);

    return 0;
}