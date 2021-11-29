#include<stdio.h>
#include <curses.h>
#include <string.h>
#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>

#include "fon.h"     		/* Primitives de la boite a outils */
#include "commands.h"
#include "List.h"

#define SERVICE_DEFAUT "1111"
#define MAX_CLIENTS 32

void serveur_appli (char *service);   /* programme serveur */


/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/
	
	serveur_appli(service);
}

typedef struct{
	struct sockaddr_in* addrin;
	int id;
	char* name;
} Client;


int match_id(Client* c, int id){
	if(c->id == id)
		return 1;
	
	return 0;
}

int match_name(Client* c, char* name){
	if(strcmp(c->name, name) == 0)
		return 1;
	
	return 0;
}

void build_fd_sets(int listen_sock, List anonymous, List users, fd_set* fds){
	
	FD_ZERO(fds);
	FD_SET(listen_sock, fds);


	Element* e = anonymous.first;
	while(e){
		FD_SET(((Client*)e->content)->id, fds);
		e = e->next;
	}

	e = users.first;
	while(e){
		FD_SET(((Client*)e->content)->id, fds);
		e = e->next;
	}
}  


void new_user(){

}

void new_connection(int listen_sock, List* anonymous){
	Client* c = malloc(sizeof(Client));
	int id_socket_client = h_accept(listen_sock, c->addrin);
	c->id = id_socket_client;
	char com = Q_NAME;
	add(anonymous, c);
	h_writes(c->id, &com, 1);
}

void handle_msg_anon(int sock_id, List* anonymous, List* users){
	int8_t command;
	h_reads(sock_id, &command, 1);
	
	switch(command){

		A_NAME:
			int length;
			h_reads(sock_id, (char*)&length, 2);
			char* name = malloc(length+1);
			h_reads(sock_id, name, length);
			name[length] = '\0';

			Client* c = find(anonymous, match_id, &sock_id);
			del(anonymous, match_id, &sock_id);
			c->name = name;
			add(users, c);
		break;

		MSG_TO:
			int8_t c = ERROR;
			char msg[] = "You must be logged in (name required)";
			int length = sizeof(msg) - 1;
			hwrites(sock_id, &c, 1);
			h_writes(sock_id, &length, 2);
			h_writes(sock_id, msg, length);
		break;

		BYE:

		break;

		default:


	}
}

void handle_msg_user(){

}

void serveur_appli(char *service)
{
	struct sockaddr_in* local;
	List anonymous;
	List users;
	
	char buffer[1024];

	int listen_sock = h_socket(AF_INET, SOCK_STREAM);
	adr_socket(service, "*", SOCK_STREAM, &local);
	h_bind(listen_sock, local);
	h_listen(listen_sock, 10);

	int maxsock = listen_sock;

	while(1){
		
		fd_set set, setbis ; /* set et setbis sont des ensembles de descripteur */
		
		build_fd_sets(listen_sock, anonymous, users, set);
		bcopy ( (char*) &set, (char*) &setbis, sizeof(set)) ; /* sauvegarde set dans setbis car set va changer lors du select */
		
		select (maxsock, &set, 0, 0, 0) ;

			
		if (FD_ISSET(listen_sock, &set)){
			new_connection(listen_sock, &anonymous);
		}

		Element* e = anonymous.first;
		while(e){
			Client* client = e->content;
			if(FD_ISSET(client->id, &set))
				handle_msg_anon(client->id, &anonymous, &users);
			e = e->next;
		}

		e = users.first;
		while(e){
			Client* client = e->content;
			if(FD_ISSET(client->id, &set))
				handle_msg_user(client->id);
			e = e->next;
		}
		
		bcopy ( (char*) &setbis, (char*) &set, sizeof(set)) ;
	}

}

