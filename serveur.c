#include<stdio.h>
#include <curses.h>
#include <string.h>
#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

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
	h_reads(sock_id, (char*)len, 4);
	char* r = malloc(*len+1);
	h_reads(sock_id, r, *len);
	r[*len] = '\0';
	return r;
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
void new_connection(int listen_sock, List* anonymous){
	Client* c = malloc(sizeof(Client));
	int id_socket_client = h_accept(listen_sock, c->addrin);
	c->id = id_socket_client;
	//printf("a %ld\n", (size_t)c->addrin);
	//printf("[NEW CONNECTION]: %s - ID=%d\n", inet_ntoa(c->addrin->sin_addr), c->id);
	printf("[NEW CONNECTION]\n");
	add(anonymous, c);
	
	char com = Q_NAME;
	h_writes(c->id, &com, 1);

}

void free_client(int sock_id, List* l){
	Client* c = (Client*)del(l,match_id, &sock_id);
	if(strlen(c->name))
		printf("[DISCONNECT]: %s (ID=%d)\n", c->name, c->id);
	else
		printf("[DISCONNECT]: ID=%d\n", c->id);
	free(c->name);
	h_close(sock_id);
	
}

void handle_msg_anon(int sock_id, List* anonymous, List* users){
	commands_e command;
	h_reads(sock_id, (char*)&command, 1);
	int length;
	switch(command){

		case A_NAME: ;
			int nl;
			char* name = read_string(sock_id, &nl);
			

			Client* c = find(anonymous, match_id, &sock_id);
			
			del(anonymous, match_id, &sock_id);
			c->name = name;
			add(users, c);
			
			printf("[USER LOGIN]: %s is %s\n", inet_ntoa(c->addrin->sin_addr), c->name);
			break;

		case MSG_TO:
			clear_buffer(sock_id);
			
			int8_t com = ERROR;
			char err_msg[] = "You must be logged in to send messages (name required)";
			length = sizeof(err_msg) - 1;
			h_writes(sock_id, &com, 1);
			h_writes(sock_id, (char*)&length, 4);
			h_writes(sock_id, err_msg, length);
			break;

		BYE:
			free_client(sock_id, anonymous);
			break;

		default:
			clear_buffer(sock_id);

			int8_t com_ = ERROR;
			char err_msg_[] = "Unknown command";
			length = sizeof(err_msg_) - 1;
			h_writes(sock_id, &com_, 1);
			h_writes(sock_id, (char*)&length, 4);
			h_writes(sock_id, err_msg, length);
			break;
	}
}

void send_msg(Client* src, Client* dest, char* msg, int msg_len){

	printf("[MSG]: %s -> %s\n", src->name, dest->name);

	int8_t command = MSG_FROM;
	int name_len = strlen(src->name);
	h_writes(dest->id, &command, 1);
	h_writes(dest->id, (char*)&name_len, 4);
	h_writes(dest->id, src->name, name_len);
	h_writes(dest->id, (char*)&msg_len, 4);
	h_writes(dest->id, msg, msg_len);
}

void handle_msg_user(int sock_id, List* users){
	commands_e command;
	h_reads(sock_id, (char*)&command, 1);
	
	
	int length;

	switch(command){

		case A_NAME:
			clear_buffer(sock_id);

			int8_t c = ERROR;
			char err_msg[] = "Already logged in";
			length = sizeof(err_msg) - 1;
			h_writes(sock_id, &c, 1);
			h_writes(sock_id, (char*)&length, 4);
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
				h_writes(sock_id, (char*)&length, 4);
				h_writes(sock_id, msg, length);
			}
			break;

		case BYE:
			free_client(sock_id, users);
			break;

		default:
			clear_buffer(sock_id);

			int8_t com = ERROR;
			char err_msg_[] = "Unknown command";
			length = sizeof(err_msg_) - 1;
			h_writes(sock_id, &com, 1);
			h_writes(sock_id, (char*)&length, 4);
			h_writes(sock_id, err_msg, length);
			break;
	}
}


int get_maxsock_l(List* l){
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
int get_maxsock(int listen_sock, List* anon, List* users){
	int maxsock = listen_sock;
	int max1 = get_maxsock_l(anon);
	int max2 = get_maxsock_l(users);

	if(max1 > maxsock)
		maxsock = max1;
	if(max2 > maxsock)
		maxsock = max2;
	return maxsock + 1;
}
void serveur_appli(char *service)
{
	struct sockaddr_in* local;
	List anonymous;
	List users;
	init(&anonymous);
	init(&users);

	printf("<<<< PolyRTC v1 >>>>\n");
	//====== socket d'écoute ======
	int listen_sock = h_socket(AF_INET, SOCK_STREAM);
	
	adr_socket(service, "*", SOCK_STREAM, &local);
	h_bind(listen_sock, local);
	h_listen(listen_sock, 10);
	//==============================

	printf("Listening on %s\n", service);
	//boucle principale
	while(1){
		int maxsock = get_maxsock(listen_sock, &anonymous, &users);

		fd_set set, setbis ; /* set et setbis sont des ensembles de descripteur */
		
		build_fd_sets(listen_sock, anonymous, users, &set); //on remplis le set

		bcopy ( (char*) &set, (char*) &setbis, sizeof(set)) ; /* sauvegarde set dans setbis car set va changer lors du select */

		printf("maxsock %d\n", maxsock);
		//en attente d'un évenement
		select (maxsock, &set, 0, 0, 0) ;
		printf("up\n");

		//nouvelle connexion ?
		if (FD_ISSET(listen_sock, &set)){
			new_connection(listen_sock, &anonymous);
		}
		
		//message reçu d'un client anonyme ?
		Element* e = anonymous.first;
		while(e){
			Client* client = e->content;
			if(FD_ISSET(client->id, &set))
				handle_msg_anon(client->id, &anonymous, &users);
			e = e->next;
		}

		//message reçu dd'un utilisateur ?
		e = users.first;
		while(e){
			Client* client = e->content;
			if(FD_ISSET(client->id, &set))
				handle_msg_user(client->id, &users);
			e = e->next;
		}
		
		//reset du fd_set
		bcopy ( (char*) &setbis, (char*) &set, sizeof(set)) ;
	}

}

