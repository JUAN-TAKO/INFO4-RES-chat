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


#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */


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
	scanf("%s", serveur);

	printf("\nNuméro de port: ");
	scanf("%s", service);

	printf("\nPseudonyme souhaité sur la messagerie (32 charactères max): ");
	char* pseudo = malloc(32);
	scanf("%s", pseudo);

	*sock_num = h_socket(AF_INET, SOCK_STREAM);
	struct sockaddr_in *adrClient;
	adr_socket(service, serveur, SOCK_STREAM, &adrClient);
	h_connect(*sock_num, adrClient);

	return pseudo;
}


void send_pseudo(int sock_num, char* pseudo){
	char* tampon = malloc(1);
	h_reads(sock_num, tampon, 1);
	if (strcmp(tampon, (char*)Q_NAME)) h_writes(sock_num, (char*)A_NAME, 1);
}

void display_help(){
	printf("test");
}

void start_communication(int sock_num){
	display_help();
}

void send_bye(int sock_num){
	int8_t command = BYE;
	h_writes(sock_num, &command, 1);
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

	start_communication(sock_num);



	/* Fermeture de la connection */
	send_bye(sock_num);
	h_close(sock_num);
}

/*****************************************************************************/

