build:
	gcc tema2.c TCoada.c TStiva.c utils.c -Wall -Wextra -std=c99 -o tema2

run:
	./tema2 input.txt out
clear:
	rm -f *.o
