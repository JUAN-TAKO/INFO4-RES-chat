#include "stdio.h"
#include "stdlib.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    printf("Hello\n");
    void* addr;
    int status;

    char ipstr[INET6_ADDRSTRLEN], ipver;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo("5000", "im2ag-mandelbrot.univ-grenoble-alpes.fr", &hints, &res);

    while(res != NULL){
        if(res->ai_family == AF_INET){
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)res->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver=4;
        }
        else{
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)res->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver=6;
        }

        inet_ntop(res->ai_family, addr, ipstr, sizeof ipstr);
        printf(" IPv%c: %s\n", ipver,ipstr);

        res = res->ai_next;
    }
    freeaddrinfo(res);
    return 0;
}
