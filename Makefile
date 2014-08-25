CC = gcc -fopenmp
DBG = -g -ggdb
WFLG = -Wall -pedantic -Wunused -Wformat -Wextra -Wunreachable-code -Wformat-security -Winline -Wreturn-type -Wuninitialized -Wimplicit-function-declaration -Wnonnull
INCL_H = include/
INCL_O = objects/
INCL_S = sources/

plist: $(INCL_O)main.o $(INCL_O)plist.o $(INCL_O)plist_t.o $(INCL_O)plist_err.o
	$(CC) $(INCL_O)main.o $(INCL_O)plist.o $(INCL_O)plist_t.o $(INCL_O)plist_err.o -o $@

$(INCL_O)main.o: $(INCL_S)main.c $(INCL_H)plist.h $(INCL_H)plist_tests.h
	$(CC) -c $(DBG) $(WFLG) $(INCL_S) $< -o $@

$(INCL_O)plist.o: $(INCL_S)plist.c $(INCL_H)plist.h $(INCL_H)plist_err.h
	$(CC) -c $(DBG) $(WFLG) $(INCL_S) $< -o $@

$(INCL_O)plist_t.o: $(INCL_S)plist_tests.c $(INCL_H)plist.h $(INCL_H)plist_tests.h
	$(CC) -c $(DBG) $(WFLG) $(INCL_S) $< -o $@

$(INCL_O)plist_err.o: $(INCL_S)plist_err.c $(INCL_H)plist_err.h
	$(CC) -c $(DBG) $(WFLG) $(INCL_S) $< -o $@

clean:
	rm objects/* plist
