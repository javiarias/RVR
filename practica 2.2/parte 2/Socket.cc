#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port)
{
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo.
    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase

    struct addrinfo hints;
    struct addrinfo * res;

    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family   = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int rc = getaddrinfo(address, port, &hints, &res);

    if ( rc != 0 )
    {
        std::cerr << gai_strerror(rc) << std::endl;
        return;
    }

    sa = *res->ai_addr;
    sa_len = res->ai_addrlen;
    sd = socket(res->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    try
    {
        obj.to_bin();

        char* dat = obj.data();
        sendto(sd, dat, obj.size(), 0, &sock.sa, sock.sa_len);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
    
    return 0;
}

bool operator== (const Socket &s1, const Socket &s2)
{
    sockaddr sock1 = s1.sa;
    sockaddr sock2 = s2.sa;
    sockaddr_in* sin1;
    sockaddr_in* sin2;

    try
    {
        sin1 = reinterpret_cast<sockaddr_in*>(&sock1);
        sin2 = reinterpret_cast<sockaddr_in*>(&sock2);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }    

    bool ret = true;

    ret = ret && (sin1->sin_family == sin2->sin_family) && (sin1->sin_port == sin2->sin_port) && (sin1->sin_addr.s_addr == sin2->sin_addr.s_addr);

    return ret;
};

bool operator!= (const Socket &s1, const Socket &s2)
{
    return !operator==(s1, s2);
};

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};

