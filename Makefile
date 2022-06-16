#Définition des macros
CC = gcc
CFLAGS = -Wall -c -g
LDFLAGS = -lm
CSRC = *.c
OBJS = main.o\
    init.o chemins_gloutons.o enchere.o generateur.o

run : main
	./main
	
.PHONY: run
run: main
	echo $(CSRC)
	./main
	
debug: main
	valgrind --leak-check=full --show-leak-kinds=all ./main
	
main: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	
main.o: main.c *.h
	$(CC) $(CFLAGS) $<

%.o: %.c %.h structure.h
	$(CC) $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f *.o
	rm -f main
	ls -l