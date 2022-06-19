#Définition des macros
CC = gcc
CFLAGS = -Wall -c -g
LDFLAGS = -Wall -g
CSRC = *.c
OBJS = main.c\
	init.o chemins_gloutons.o enchere.o generateur.o affichage.o

.PHONY: run
run: main
	echo $(CSRC)
	./main
	
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