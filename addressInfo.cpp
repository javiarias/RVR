#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
 
#include <iostream>

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * res;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_UNSPEC;
    
    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    
    if ( rc != 0 )
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    std::cout << "HOST" << "  ---  " << "FAMILY" << "  ---  " << "SOCKET TYPE" << std::endl;

    for(int i = 0; res != NULL; res = res->ai_next){
    
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        
        getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, service,
        NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << host << "  ---  " << res->ai_family << "  ---  " << res->ai_socktype << std::endl;
    }


    
    freeaddrinfo(res);
    
    return 0;
}