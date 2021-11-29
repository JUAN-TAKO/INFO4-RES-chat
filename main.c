#include "stdio.h"
#include "stdlib.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

void adr_socket ( char *service, char *nom,
		  struct sockaddr_in **p_adr_serv)
{
    struct addrinfo hints; /* info à passer a getaddrinfo */
    struct addrinfo *res; /* permet de recuperer les adresses a l aide de getaddrinfo */
    char ipstr[INET6_ADDRSTRLEN], ipver;
    int s;
    void *addr;
    

	/* RENSEIGNEMENT ADRESSES SOCKET  ------------------------------*/

	/* Mise a zero de la structure d'adresse socket */
    memset(&hints, 0, sizeof(struct addrinfo));

	/* Definition du domaine ( famille ) 	*/
    /*hints.ai_family = AF_UNSPEC; */   /* Allow IPv4 or IPv6 */
    hints.ai_family = AF_UNSPEC; /* on force  IPV4 */
    hints.ai_socktype = SOCK_STREAM; /* SOCK_DGRAM (UDP) ou SOCK_STREAM (TCP) */
    hints.ai_protocol = 0;          /* Any protocol */

	/* ------ RENSEIGNE @IP -----------------------------------------*/
        if (nom==NULL) /* Cas d'un serveur */
          hints.ai_flags = AI_PASSIVE;
        else
            hints.ai_flags = 0;

	s = getaddrinfo(nom, service, &hints, &res);
    if (s != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    *p_adr_serv=(struct sockaddr_in *)res->ai_addr;

    while(res != NULL){
        printf("addr\n");
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
        printf(" IPv%d: %s\n", ipver,ipstr);

        res = res->ai_next;
    }
    freeaddrinfo(res);
}

int main(int argc, char const *argv[])
{
    printf("Hello\n");

    struct sockaddr_in* sa;
    adr_socket("80", "google.com", &sa);
    
    printf("Done\n");
    
    return 0;
}
