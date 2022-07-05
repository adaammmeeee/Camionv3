#Définition des macros
CC = gcc
CFLAGS = -Wall -c -g
LDFLAGS = -Wall -g -lm
CSRC = *.c
OBJS = main.c\
	structures.o init.o chemins_gloutons.o enchere.o generateur.o brute_force.o

.PHONY: run
run: main
	echo $(CSRC)
	./main "matrice_distance.csv"

test: main
	echo $(CSRC)
	./main "matrice_distance_10_sommets.csv"

brute: main
	echo $(CSRC)
	./main "matrice_distance.csv" brute

brute_test: main
	echo $(CSRC)
	./main "matrice_distance_10_sommets.csv" brute

debug: main
	valgrind --leak-check=full --show-leak-kinds=all ./main "matrice_distance.csv"
	
main: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 

%.o: %.c %.h structure.h
	$(CC) $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f *.o
	rm -f main
	rm -f gestionnaire
	rm -f resultat_*
	rm -f analyse*
	rm -f trajet
	ls