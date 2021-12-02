
OBJ1 = fon.o client.o List.o
OBJ2 = fon.o serveur.o List.o
OPTIONS	= 
CFLAGS = -g
LFLAGS = -g
# Adaptation a Darwin / MacOS X avec fink
# Du fait de l'absence de libtermcap on se fait pas mal
# cracher dessus mais ca marche...
ifeq ($(shell uname), Darwin)
LFLAGS	+= -L/opt/local/lib
CFLAGS	+= -I /opt/local/include
endif


EXEC = ${OBJ1} client ${OBJ2} serveur
all: ${EXEC} 	

List.o : List.h List.c
	gcc -c List.c

fon.o :  fon.h fon.c
	#gcc -DDEBUG -c fon.c
	gcc -c fon.c

client.o : fon.h	client.c 
	gcc  $(CFLAGS) -c  client.c	-o client.o

serveur.o : fon.h	serveur.c 
	gcc  $(CFLAGS) -c  serveur.c -o serveur.o

client : ${OBJ1}	
	gcc $(LFLAGS) ${OBJ1} -o client  $(OPTIONS)

serveur : ${OBJ2}	
	gcc $(LFLAGS) ${OBJ2} -o serveur  $(OPTIONS)



clean : 
	rm -f ${EXEC} core essaicurse

