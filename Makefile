make default: lab4b.c
	gcc -lm -lmraa -Wall -Wextra -lpthread lab4b.c -o lab4b
make check:
	echo "success"
make clean:
	rm lab4b *.txt *.tar.gz
make dist:
	tar -cvzf lab4b-304815342.tar.gz *.c Makefile README