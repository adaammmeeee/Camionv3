#Définition des macros
CC = gcc
CFLAGS = -Wall -c -g
LDFLAGS = -Wall -g -lm
CSRC = *.c
OBJS = main.c\
	structures.o init.o chemins_gloutons.o enchere.o generateur.o brute_force.o

.PHONY: run
all_insert: main
	echo $(CSRC)
	./main "matrice_distance.csv" insertion sans
	./main "matrice_distance.csv" insertion confiance
	./main "matrice_distance.csv" insertion enchere2


all_insert_test: main
	echo $(CSRC)
	./main "matrice_distance_10_sommets.csv"

brute: main
	echo $(CSRC)
	./main "matrice_distance.csv" brute

brute_test: main
	echo $(CSRC)
	./main "matrice_distance_10_sommets.csv" brute

insert_sans: main
	./main "matrice_distance.csv" insertion sans

insert_enchere: main
	./main "matrice_distance.csv" insertion enchere1

insert_confiance: main
	./main "matrice_distance.csv" insertion confiance

fin_sans: main
	./main "matrice_distance.csv" fin sans

fin_enchere: main
	./main "matrice_distance.csv" fin enchere1

fin_confiance: main
	./main "matrice_distance.csv" fin confiance

comparaison_echange: main
	./main "matrice_distance.csv" insertion sans 100
	./main "matrice_distance.csv" insertion enchere0 100
	./main "matrice_distance.csv" insertion confiance 100

marge: main
	./main "matrice_distance.csv" insertion enchere0 100
	./main "matrice_distance.csv" insertion enchere1 100
	./main "matrice_distance.csv" insertion enchere2 100

debug: main
	valgrind --leak-check=full --show-leak-kinds=all ./main "matrice_distance.csv" insertion sans
	valgrind --leak-check=full --show-leak-kinds=all ./main "matrice_distance.csv" insertion enchere1
	valgrind --leak-check=full --show-leak-kinds=all ./main "matrice_distance.csv" insertion confiance
	
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