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


int match_id(void* client, void* id){
	Client* c = client;
	int idm = *((int*) id);
	if(c->id == idm)
		return 1;
	
	return 0;
}

int match_name(void* client, void* name){
	Client* c = client;
	char* namem = (char*) name;
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

char* read_string(int sock_id, int* len){
	h_reads(sock_id, (char*)len, 2);
	return malloc(*len+1);
}
void skip_string(int sock_id){
	int len;
	h_reads(sock_id, (char*)&len, 2);
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
	commands_e command;
	h_reads(sock_id, (char*)&command, 1);
	int length;
	switch(command){

		case A_NAME:
			h_reads(sock_id, (char*)&length, 2);
			char* name = malloc(length+1);
			h_reads(sock_id, name, length);
			name[length] = '\0';

			Client* c = find(anonymous, match_id, &sock_id);
			del(anonymous, match_id, &sock_id);
			c->name = name;
			add(users, c);
		break;

		case MSG_TO:
			skip_string(sock_id); //name
			skip_string(sock_id); //msg	

			int8_t com = ERROR;
			char err_msg[] = "You must be logged in (name required)";
			length = sizeof(err_msg) - 1;
			h_writes(sock_id, &com, 1);
			h_writes(sock_id, (char*)&length, 2);
			h_writes(sock_id, err_msg, length);
		break;

		case BYE:

		break;

		default:

		break;
	}
}

void send_msg(Client* src, Client* dest, char* msg, int msg_len){
	int8_t command = MSG_FROM;
	int name_len = strlen(src->name);
	h_writes(dest->id, &command, 1);
	h_writes(dest->id, (char*)&name_len, 2);
	h_writes(dest->id, src->name, name_len);
	h_writes(dest->id, (char*)&msg_len, 2);
	h_writes(dest->id, msg, msg_len);
}

void handle_msg_user(int sock_id, List* users){
	commands_e command;
	h_reads(sock_id, (char*)&command, 1);
	
	
	int length;

	switch(command){

		case A_NAME:
			skip_string(sock_id); //name

			int8_t c = ERROR;
			char err_msg[] = "Already logged in";
			length = sizeof(err_msg) - 1;
			h_writes(sock_id, &c, 1);
			h_writes(sock_id, (char*)&length, 2);
			h_writes(sock_id, err_msg, length);
			break;

		case MSG_TO: ;
			int name_len;
			char* name = read_string(sock_id, &name_len);

			int msg_len;
			char* msg = read_string(sock_id, &msg_len);

			Client* dest = find(users, match_name, name);
			Client* src = find(users, match_id, &sock_id);

			if(dest){
				send_msg(src, dest, msg, msg_len);
			}
			else{
				int8_t c = ERROR;
				char msg[] = "User not found";
				length = sizeof(msg) - 1;
				h_writes(sock_id, &c, 1);
				h_writes(sock_id, (char*)&length, 2);
				h_writes(sock_id, msg, length);
			}
			break;

		case BYE:

			break;

		default:

			break;
	}
}


int get_maxsock(List* l){
	int max = 0;
	Element* e = l->first;
	while(e){
		Client* client = e->content;
		if(client->id > max)
			max = client->id;
		e = e->next;
	}
	return max;
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

	while(1){
		int maxsock = listen_sock;
		int max1 = get_maxsock(&anonymous);
		int max2 = get_maxsock(&users);
		if(max1 > maxsock)
			maxsock = max1;
		if(max2 > maxsock)
			maxsock = max2;


		fd_set set, setbis ; /* set et setbis sont des ensembles de descripteur */
		
		build_fd_sets(listen_sock, anonymous, users, &set);
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
				handle_msg_user(client->id, &users);
			e = e->next;
		}
		
		bcopy ( (char*) &setbis, (char*) &set, sizeof(set)) ;
	}

}

