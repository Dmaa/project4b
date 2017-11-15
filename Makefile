default: lab4b.c
	gcc -lm -lmraa -Wall -Wextra -lpthread lab4b.c -o lab4b
check:
	echo "success"
clean:
	rm -f *.txt *.tar.gz
dist:
	tar -cvzf lab4b-304815342.tar.gz *.c Makefile README