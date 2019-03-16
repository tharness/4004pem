4004pem: interface.o
	gcc interface.o -o 4004pem

interface.o: interface.c
	gcc -c interface.c