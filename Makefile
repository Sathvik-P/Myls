all:	myls

myls:	myls.o
	@gcc -g -Wall -O3 -o myls myls.o

myls.o:	myls.c
	@cc -c myls.c

clean:
	@rm -f myls myls.o
