4004pem: interface.o emulate.o
	gcc interface.o emulate.o -o 4004pem -lpthread

interface.o: interface.c
	gcc -c interface.c

emulate.o: emulate.c
	gcc -c emulate.c

clean:
	rm -f *.o