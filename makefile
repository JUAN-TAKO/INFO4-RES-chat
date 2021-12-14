
OBJ1 = netutils.o client.o List.o
OBJ2 = netutils.o serveur.o List.o
OPTIONS	= 
CFLAGS = -g
LFLAGS = -g



EXEC = ${OBJ1} client ${OBJ2} serveur
all: ${EXEC} 	

List.o : List.h List.c
	gcc -c List.c

netutils.o :  netutils.h netutils.c
	#gcc -DDEBUG -c netutils.c
	gcc -c netutils.c

client.o : netutils.h	client.c 
	gcc  $(CFLAGS) -c  client.c	-o client.o

serveur.o : netutils.h	serveur.c 
	gcc  $(CFLAGS) -c  serveur.c -o serveur.o

client : ${OBJ1}	
	gcc $(LFLAGS) ${OBJ1} -o client  $(OPTIONS)

serveur : ${OBJ2}	
	gcc $(LFLAGS) ${OBJ2} -o serveur  $(OPTIONS)



clean : 
	rm -f ${EXEC} core essaicurse

