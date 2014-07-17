CC = gcc
DBG = -g -ggdb
WFLG = -Wall -pedantic -Wunused -Wformat -Wextra -Wunreachable-code -Wformat-security -Winline -Wreturn-type -Wuninitialized -Wimplicit-function-declaration

plist : main.c
	$(CC) $(DBG) $(WFLG) main.c -o plist

tests: t1 t2

t1:
	echo "Executando teste 1"

t2:
	echo "Executando teste 2"
	

clean:
	rm *.o plist
