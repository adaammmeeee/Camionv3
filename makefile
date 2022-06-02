run: compil
	./main
	
debug: compil
	valgrind --leak-check=full --show-leak-kinds=all ./main

lib.o: lib.c lib.h
	gcc -Wall -c -g lib.c 

main.o: main.c lib.h
	gcc -Wall -c -g main.c 

compil: lib.o main.o
	gcc -Wall -o main main.o lib.o

clean: 
	rm -f *.o main a.out