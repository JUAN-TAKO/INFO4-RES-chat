#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include "netutils.h"   		/* primitives de la boite a outils */
#include "commands.h"

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur = malloc(16); 
	char *service = malloc(8);


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	client_appli(serveur,service);
}

char* init_connection(char *serveur, char *service, int *sock_num){

	printf("\n################\n");
	printf("Veuillez entrer les informations suivantes:\n");

	printf("Adresse IP (ou nom) du serveur: ");
	scanf("%16s", serveur);

	printf("\nNuméro de port: ");
	scanf("%8s", service);

	printf("\nPseudonyme souhaité sur la messagerie (32 caractères max): ");
	char* pseudo = malloc(32);
	scanf("%32s", pseudo);

	*sock_num = h_socket(AF_INET, SOCK_STREAM);
	struct sockaddr_in *adrClient;
	adr_socket(service, serveur, SOCK_STREAM, &adrClient);
	h_connect(*sock_num, adrClient);

	return pseudo;
}



void send_pseudo(int sock_num, char* pseudo){
	char tampon;
	h_reads(sock_num, &tampon, 1);
	commands_e cm = A_NAME;
	if ((uint8_t)tampon == (uint8_t)Q_NAME){
		h_writes(sock_num, (char*)&cm, 1);
		write_string(sock_num, pseudo);
	} 
}

void display_help(){
	printf("commandes: \n");
	printf("  envoi -> envoyer un message\n");
	printf("  list -> liste des utilisateurs\n");
	printf("  quit -> quitter\n");
	printf("\n");

}

void stdin_ignore(){
	char c;
	while (true) {
        c = getchar();
        if (c == EOF)
           break;
		if(c == '\n')
			break;
    }
}

void communication(int sock_num, char* command, char* name, char* message, fd_set* set){
	scanf("%8s", command);
	

	int case_number=-1;
	if (strcmp(command,"envoi")==0) case_number = 0;
	else if (strcmp(command,"list")==0) case_number = 1;
	else if (strcmp(command,"quit")==0) case_number = 2;
	else {
		printf("commande inconnue\n");
		display_help();
	}

	commands_e cm;
	switch (case_number)
	{
	case 0:
		printf("Entrer le nom du destinataire: ");
		scanf("%32s", name);
		printf("Entrez le message: ");
		size_t t = 0;
		stdin_ignore();
		getline(&message, &t, stdin);

		write_command(sock_num, MSG_TO);
		write_string(sock_num, name);
		write_string(sock_num, message);
		break;
	case 1:
		write_command(sock_num, Q_LIST);
		break;
	case 2:
		send_bye(sock_num);
		free(command);
		free(name);
		free(message);
		h_close(sock_num);
		break;
	default:
		break;
	}


}

void reception(int sock_num, char* pseudo){
	
	commands_e com;
	if (read(sock_num, &com, 1) == 0) return;
	else {
		int len;
		char* name;
		char* msg;
		com = com & 0xFF;

		switch(com){
			case Q_NAME:
				write_command(sock_num, A_NAME);
				write_string(sock_num, pseudo);
				break;

			case MSG_FROM:
				name = read_string(sock_num, &len);
				msg = read_string(sock_num, &len);
				printf("<%s>: ", name);
				printf("%s\n", msg);
				free(name);
				free(msg);
				break;
			
			case A_LIST:
				printf("Online: \n");
				h_reads(sock_num, (char*)&len, 4);
				int tmp;
				for(int i=0; i < len; i++){
					name = read_string(sock_num, &tmp);
					printf("  %s\n", name);
					free(name); 
				}
				printf("----------\n");
				break;

			case INFO: ;
				msg = read_string(sock_num, &len);
				printf("[INFO]: %s\n", msg);
				free(msg);
				break;
			
			case ERROR: 
				msg = read_string(sock_num, &len);
				printf("[ERROR]: %s\n", msg);
				free(msg);
				break;
		}
	}
}

/*****************************************************************************/
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */

{
	/* Initialisation de la connection */
	int sock_num;
	char* pseudo = init_connection(serveur, service, &sock_num);

	char* command = malloc(8);
	char* name = malloc(32);
	char* message = malloc(2048);

	display_help();
	while(1){
		fd_set set, setbis;
		FD_ZERO(&set);
		FD_SET(1, &set); //terminal, num socket ? 
		FD_SET(sock_num, &set);
	
		select(sock_num+1, &set, 0, 0, 0);
		if (FD_ISSET(1, &set))
			communication(sock_num, command, name, message, &set);
		if (FD_ISSET(sock_num, &set)) {
			reception(sock_num, pseudo);
		}
	}


}

/*****************************************************************************/

