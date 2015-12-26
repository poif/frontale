#Compilateur utilisé
CC=gcc

#Options de compilation
CFLAGS=-Wall

#Options d'edition des liens
LDFLAGS=-lssl -lcrypto -lmysqlclient -lpthread

prog_bdd: main.o bdd.o bdd_request.o crypt.o res.o
	$(CC) $(CFLAGS) -o prog_bdd *.o $(LDFLAGS)

main.o: main.c default.h bdd.h crypt.h res.h
	$(CC) -c main.c $(CFLAGS)

bdd.o: bdd.c bdd.h default.h res.h
	$(CC) -c bdd.c $(CFLAGS)

bdd_request.o: bdd_request.c bdd.h default.h crypt.h res.h
	$(CC) -c bdd_request.c $(CFLAGS)

crypt.o: crypt.c crypt.h default.h
	$(CC) -c crypt.c $(CFLAGS)

res.o: res.c res.h default.h bdd.h
	$(CC) -c res.c $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf prog_bdd
