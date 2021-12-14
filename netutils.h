#include <stdio.h>
#include<unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

/*------------------ CAS des prises TCP/IP ----------*/
#include <netinet/in.h>
#include <netdb.h>
#include "commands.h"

void write_string(int sock_id, char* msg);

char* read_string(int sock_id, int* len);

void send_bye(int sock_num);

void skip_string(int sock_id);

void clear_buffer(int sock_id);

void write_command(int sock_id, commands_e command);

commands_e read_command(int sock_id);


/*			    +===================+			  	*/
/*==========================|	H_SOCKET	|===============================*/
/*			    +===================+				*/
int h_socket ( int domaine, int mode );

/*			    +===================+				*/
/*==========================|	H_BIND		|===============================*/
/*			    +===================+				*/
void h_bind ( int num_soc, struct sockaddr_in *p_adr_socket );

/*			    +===================+				*/
/*==========================| 	H_CONNECT	|===============================*/
/*			    +===================+				*/
void h_connect( int num_soc, struct sockaddr_in *p_adr_serv );

/*			    +===================+				*/
/*==========================|  H_LISTEN  	|===============================*/
/*			    +===================+				*/
void h_listen ( int num_soc, int nb_req_att );

/*			    +===================+				*/
/*==========================| H_ACCEPT 		|===============================*/
/*			    +===================+				*/
int h_accept( int num_soc, struct sockaddr_in *p_adr_client );

/*			    +===================+				*/
/*==========================| 	H_READS		|===============================*/
/*			    +===================+				*/
int h_reads ( int num_soc, char *tampon, int nb_octets );

/*			    +===================+				*/
/*==========================|	H_WRITES	|===============================*/
/*			    +===================+				*/
int h_writes ( int num_soc, char *tampon, int nb_octets );


/*			    +===================+				*/
/*==========================|	H_CLOSE		|===============================*/
/*			    +===================+				*/
void h_close ( int socket ); 

/*			    +===================+				*/
/*==========================|	ADR_SOCKET	|===============================*/
/*			    +===================+				*/
void adr_socket( char *service, char *serveur, int typesock,
		 struct sockaddr_in **p_adr_serv);
