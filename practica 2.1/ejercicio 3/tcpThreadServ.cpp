#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

class ConexionTCP
{
    public:
        ConexionTCP(int sd_client, int thid) : sd_client(sd_client), thid(thid)
        {};
        ~ConexionTCP() {};
        
        void haz_conexion()
            {
                while(true)
                {
                    // ---------------------------------------------------------------------- //
                    // GESTION DE LA CONEXION CLIENTE //
                    // ---------------------------------------------------------------------- //
                    char buffer[80];

                    ssize_t bytes = recv(sd_client, (void *) buffer, sizeof(char)*79, 0);

                    if ( bytes <= 0 )
                    {
                        return;
                    }

                    buffer[bytes] = '\0';

                    std::cout << "[" << thid << "] MENSAJE: " << buffer << std::endl;

                    send(sd_client, (void *) buffer, bytes, 0);
                }

                // FIN CONEXION
                close(sd_client);
            };

    private:
        int sd_client, thid;

};


int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * res;

    // ---------------------------------------------------------------------- //
    // INICIALIZACIÓN SOCKET & BIND //
    // ---------------------------------------------------------------------- //

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

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
        std::cerr << "bind: " << std::endl;
        return -1;
    }

    freeaddrinfo(res);

    // ---------------------------------------------------------------------- //
    // PUBLICAR EL SERVIDOR (LISTEN) //
    // ---------------------------------------------------------------------- //
    listen(sd, 16);

    // ---------------------------------------------------------------------- //
    // GESTION DE LAS CONEXIONES AL SERVIDOR //
    // ---------------------------------------------------------------------- //
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int th_id = 0;

    std::vector<ConexionTCP> pool_tcp_obj;
    std::vector<std::thread> pool_thread;

    while(true)
    {
        struct sockaddr client_addr;
        socklen_t client_len = sizeof(struct sockaddr);

        // Thread ppal se bloqua en accept
        int sd_client = accept(sd, &client_addr, &client_len);

        getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
        NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "CONEXION DESDE IP: " << host << " PUERTO: " << service
        << std::endl;

        //Crear thread para tratar cada conexion
        ConexionTCP tcpObj(sd_client, th_id++);
        pool_thread.push_back(std::thread(&ConexionTCP::haz_conexion, tcpObj));

        pool_tcp_obj.push_back(tcpObj);

        pool_thread.back().detach();
    }

    return 0;
}