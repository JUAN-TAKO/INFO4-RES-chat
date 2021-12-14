#include<stdio.h>
#include<unistd.h>
#include <stdarg.h>
#include <string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include "netutils.h"
#include "commands.h"

void write_string(int sock_id, char* msg){
	int len = strlen(msg);
	h_writes(sock_id, (char*)&len, 4);
	h_writes(sock_id, msg, len);
}

char* read_string(int sock_id, int* len){
	h_reads(sock_id, (char*)len, 4);
	char* r = malloc(*len+1);
	h_reads(sock_id, r, *len);
	r[*len] = '\0';
	return r;
}

void send_bye(int sock_num){
	commands_e cm = BYE;
	h_writes(sock_num, (char*)&cm, 1);
	exit(-1);
}
void skip_string(int sock_id){
	int len;
	free(read_string(sock_id, &len));
}
void clear_buffer(int sock_id){
	char buffer[256];
	int r;
	while(  (r = read(sock_id, buffer, 256)) != 0);
}
void write_command(int sock_id, commands_e command){
	h_writes(sock_id, (uint8_t*)&command, 1);
}
commands_e read_command(int sock_id){
	uint8_t c = 0;
	h_reads(sock_id, (char*)&c, 1);
	return (commands_e)c;
}

int h_socket ( int domaine, int mode )
{
	int res ;	/* Entier resultat de l'operation */
	int sendbuff;

	if (domaine!=AF_INET) printf(" Sous internet utiliser -> AF_INET\n");
	if ( (mode!=SOCK_STREAM)&&(mode!=SOCK_DGRAM)) printf(" Mode a choisir parmi : SOCK_STREAM, SOCK_DGRAM \n");

	res = socket( domaine, mode, 0 ) ;

	setsockopt(res, SOL_SOCKET, SO_REUSEADDR, (char *) &sendbuff, sizeof(sendbuff));

	if ( res < 0 ) printf ( "\nERREUR 'h_socket' : Creation socket impossible \n" );
	
	return res;
}

void h_bind ( int num_soc, struct sockaddr_in *p_adr_socket )
{

	int res;	/* Entier resultat de l'operation 	*/
	struct sockaddr_in s,s1;
	int lensa;
	char str[INET_ADDRSTRLEN];
	
	res=bind( num_soc, (struct sockaddr *)p_adr_socket, sizeof(struct
	sockaddr_in) ); /* Modif PS */

	if ( res < 0 ) printf( "\nERREUR 'h_bind' : liaison socket %d impossible\n",num_soc);

}

void h_connect( int num_soc, struct sockaddr_in *p_adr_serv)
{
	
	int res;		/* Entier resultat de l'operation 	*/
	char str[INET_ADDRSTRLEN];
	
	res = connect ( num_soc, (struct sockaddr *)p_adr_serv, sizeof(struct sockaddr) );
	
	if ( res < 0 ) {
			inet_ntop(AF_INET, &(p_adr_serv->sin_addr),str,INET_ADDRSTRLEN);
			printf( "\nERREUR 'h_connect' : connexion serveur %s impossible\n"
				   ,str);}

}

void h_listen ( int num_soc, int nb_req_att )
{
	int res;		/* Entier resultat de l'operation 	*/

	res = listen( num_soc, nb_req_att );
	if ( res < 0 ) 
	  	printf("\nERREUR 'h_listen' : ecoute sur socket %d impossible \n",num_soc );
}

int h_accept( int num_soc, struct sockaddr_in *p_adr_client )
{

	int longueur ;
	int res;			/* Entier resultat de l'operation 	*/

	longueur = sizeof( struct sockaddr );
	res = accept ( num_soc, (struct sockaddr *)p_adr_client, (socklen_t *)&longueur );
	if ( res < 0 ) 
	  printf ( "\nERREUR 'h_accept' : Acceptation impossible \n" );

	return res ;
	
}


int h_reads ( int num_soc, char *tampon, int nb_octets )
{
	int nb_restant, nb_lus;

	nb_restant=nb_octets ;
	while ( nb_restant > 0 )
	{
		nb_lus = read ( num_soc, tampon, nb_restant );

		if ( nb_lus<0 )
		{
	  		printf( "\nERREUR 'Lire_mes' : lecture socket %d impossible\n", num_soc );
			return (nb_lus);
		}

		nb_restant = nb_restant - nb_lus;

		/*if (nb_lus==0 || tampon[nb_lus]=='\0')*/		/* fin transfert */
		if (nb_lus==0){
			return (nb_octets-nb_restant);
		}
		tampon = tampon + nb_lus;

	}


	return (nb_octets-nb_restant);
}


int h_writes ( int num_soc, char *tampon, int nb_octets )
{
	int nb_restant, nb_ecrits;	/* nb octets restant a ecrire, nb octets ecrits */

	nb_restant=nb_octets ;
	while ( nb_restant > 0 )
	{
		nb_ecrits = write ( num_soc, tampon, nb_restant );
						
		if ( nb_ecrits<0 ) 
		{
				printf( "\nERREUR 'Ecrire_mes' : ecriture socket %d impossible\n", num_soc );
				return (nb_ecrits);
		}	  

		nb_restant = nb_restant - nb_ecrits;
		tampon = tampon + nb_ecrits;
	}

	return (nb_octets-nb_restant);
}


void h_close ( int num_soc )
{
	int res;		/* Entier resultat de l'operation 	*/

	res = close ( num_soc );
	if ( res < 0 ) 
	  printf( "\nERREUR 'h_close' : fermeture socket %d impossible\n",
								 num_soc );


}

void adr_socket ( char *service, char *nom, int typesock,
		  struct sockaddr_in **p_adr_serv)
{
    struct addrinfo hints; /* info à passer a getaddrinfo */
    struct addrinfo *res; /* permet de recuperer les adresses a l aide de getaddrinfo */
    char ipstr[INET6_ADDRSTRLEN], ipver;
    int s;
    void *addr;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET; /* on force  IPV4 */
    hints.ai_socktype = typesock; 
    hints.ai_protocol = 0;          /* Any protocol */

        if (nom==NULL) /*: serveur */
          hints.ai_flags = AI_PASSIVE;
        else
            hints.ai_flags = 0;

	s = getaddrinfo(nom, service, &hints, &res);
    if (s != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    *p_adr_serv=(struct sockaddr_in *)res->ai_addr;
}



