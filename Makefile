#Définition des macros
CC = gcc
<<<<<<< HEAD
CFLAGS = -Wall -c -g
LDFLAGS = -Wall -g -lm
=======
CFLAGS = -Wall -c -g 
LDFLAGS = -Wall -g 
>>>>>>> f22d548cdb1e5db2a5cc96eacdd9a72c59963b52
CSRC = *.c
OBJS = main.c\
	init.o chemins_gloutons.o enchere.o generateur.o affichage.o brute_force.o

.PHONY: run
run: main
	echo $(CSRC)
	./main

test: main
	echo $(CSRC)
	./main "matrice_distance_10_sommets.csv"
	
debug: main
	valgrind --leak-check=full --show-leak-kinds=all ./main
	
main: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 

%.o: %.c %.h structure.h
	$(CC) $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f *.o
	rm -f main
	rm -f gestionnaire
	ls