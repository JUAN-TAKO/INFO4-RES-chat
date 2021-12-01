/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/	


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include<stdlib.h>
#include <string.h>

#include "fon.h"   		/* primitives de la boite a outils */
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
<<<<<<< HEAD
	scanf("%32s", pseudo);

=======
	scanf("%s", pseudo);
>>>>>>> dd65e75647b2d6b9df8b3421f3d17f6397756640
	*sock_num = h_socket(AF_INET, SOCK_STREAM);
	struct sockaddr_in *adrClient;
	adr_socket(service, serveur, SOCK_STREAM, &adrClient);
	h_connect(*sock_num, adrClient);

	return pseudo;
}


void send_pseudo(int sock_num, char* pseudo){
<<<<<<< HEAD
	char* tampon = malloc(1);
	printf("a\n");
	h_reads(sock_num, tampon, 1);
	printf("b\n");
	if (strcmp(tampon, (char*)Q_NAME)) h_writes(sock_num, (char*)A_NAME, 1);
	printf("c\n");

=======
	char tampon;

	h_reads(sock_num, &tampon, 1);
	if ((uint8_t)tampon == (uint8_t)Q_NAME) h_writes(sock_num, (char*)A_NAME, 1);
>>>>>>> dd65e75647b2d6b9df8b3421f3d17f6397756640
}

void display_help(){
	printf("Pour envoyer un message tapper envoi, pour la liste des contacts tapper list, et quit pour quitter:\n");
}

void send_bye(int sock_num){
	int8_t command = BYE;
	h_writes(sock_num, &command, 1);
}

void communication(int sock_num, char* command, char* name, char* message){
	display_help();
	scanf("%8s", command);

	int case_number;
	if (strcmp(command,"envoi")) case_number = 0;
	else if (strcmp(command,"list")) case_number = 1;
	else if (strcmp(command,"quit")) case_number = 2;

	switch (case_number)
	{
	case 0:
		printf("Entrer le nom du destinataire:");
		scanf("%32s", name);
		printf("Entrez le message:");
		scanf("%2048s", message);
		int tmp = strlen(name);
		int tmp2 = strlen(name);

		h_writes(sock_num, (char*)&tmp,4);
		h_writes(sock_num, name, tmp);
		h_writes(sock_num, (char*)&tmp2, 4);
		h_writes(sock_num, message, tmp2);
		break;
	case 1:
		break;
	case 2:
		free(command);
		free(name);
		free(message);
		send_bye(sock_num);
		h_close(sock_num);
		break;
	default:
		break;
	}


}

char* read_string(int sock_id, int* len){
	h_reads(sock_id, (char*)len, 4);
	char* r = malloc(*len+1);
	h_reads(sock_id, r, *len);
	r[*len] = '\0';
	return r;
}

void reception(int sock_num, char* message){
	read(sock_num, message, 1);
	int tmp;
	read_string(sock_num, &tmp);
	message = read_string(sock_num, &tmp);
	printf("%s\n", message);
}

/*****************************************************************************/
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */

{
	/* Initialisation de la connection */
	int sock_num;
	char* pseudo = init_connection(serveur, service, &sock_num);
	
	/* attente du signal pour envoyer le pseudo */
	send_pseudo(sock_num, pseudo);

	char* command = malloc(8);
	char* name = malloc(32);
	char* message = malloc(2048);

	while(1){
		communication(sock_num, command, name, message);
		reception(sock_num, message);
	}


}

/*****************************************************************************/

