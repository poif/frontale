CC	=	gcc 
CFLAGS	=  	-Wall

CIBLES	=	client

LIBS = -lpthread -lcrypto

all: $(CIBLES)

clean: 
	rm *.o *.gch $(CIBLES)


client: client.o traitement_commande.o partage.o groupe.o traitement_recu.o ecoute.o crypt.o recup_config.o envoi.o
	$(CC) $(CFLAGS) -o $@ client.o partage.o groupe.o traitement_commande.o traitement_recu.o crypt.o ecoute.o envoi.o recup_config.o $(LIBS)

client.o: client.c
		$(CC) $(CFLAGS) -c client.c crypt.h traitement_commande.h traitement_recu.h client.h

traitement_commande.o: traitement_commande.c
		$(CC) $(CFLAGS) -c traitement_commande.c traitement_recu.h traitement_commande.h

ecoute.o: ecoute.c
		$(CC) $(CFLAGS) -c ecoute.c ecoute.h

envoi.o: envoi.c
		$(CC) $(CFLAGS) -c envoi.c envoi.h

recup_config.o: recup_config.c
		$(CC) $(CFLAGS) -c recup_config.c recup_config.h

crypt.o: crypt.c
		$(CC) $(CFLAGS) -c crypt.c crypt.h

traitement_recu.o: traitement_recu.c
		$(CC) $(CFLAGS) -c traitement_recu.c traitement_recu.h partage.h envoi.h

groupe.o: groupe.c
		$(CC) $(CFLAGS) -c groupe.c groupe.h

partage.o: partage.c
		$(CC) $(CFLAGS) -std=c99 -c partage.c partage.h