CC = gcc -fopenmp
OPTZ = -O0
DBG = -g -pg 
WFLG = -Wall -pedantic -Wunused -Wformat -Wextra -Wunreachable-code -Wformat-security -Winline -Wreturn-type -Wuninitialized -Wimplicit-function-declaration -Wnonnull
INCL_H = include/
INCL_O = objects/
INCL_S = sources/

plist: $(INCL_O)main.o $(INCL_O)plist.o $(INCL_O)plist_t.o $(INCL_O)plist_err.o
	$(CC) $(OPTZ) $(DBG) $(INCL_O)main.o $(INCL_O)plist.o $(INCL_O)plist_t.o $(INCL_O)plist_err.o -o $@

$(INCL_O)main.o: $(INCL_S)main.c $(INCL_H)plist.h $(INCL_H)plist_tests.h
	$(CC) -c $(OPTZ) $(DBG) $(WFLG) $< -o $@

$(INCL_O)plist.o: $(INCL_S)plist.c $(INCL_H)plist.h $(INCL_H)plist_err.h
	$(CC) -c $(OPTZ) $(DBG) $(WFLG) $< -o $@

$(INCL_O)plist_t.o: $(INCL_S)plist_tests.c $(INCL_H)plist.h $(INCL_H)plist_tests.h
	$(CC) -c $(OPTZ) $(DBG) $(WFLG) $< -o $@

$(INCL_O)plist_err.o: $(INCL_S)plist_err.c $(INCL_H)plist_err.h
	$(CC) -c $(OPTZ) $(DBG) $(WFLG) $< -o $@

clean:
	rm objects/* plist gmon.out

test: one_thread four_threads_test
	cat time_results.txt

one_thread: one_thread_test_1 one_thread_test_2 one_thread_test_3

one_thread_test_1:
	/usr/bin/time -v ./plist -l 5 -c abcdefghijklmnopqrstuvxywz -o /dev/null -t 1 -v 2>&1 | grep "TOTAL\|Elapsed" >> time_results.txt

one_thread_test_2:
	/usr/bin/time -v ./plist -l 6 -c abcdefghijklmnopqrstuvxywz -o /dev/null -t 1 -v 2>&1 | grep "TOTAL\|Elapsed" >> time_results.txt

one_thread_test_3:
	/usr/bin/time -v ./plist -l 6 -c abcdefghijklmnopqrstuvxywz0123456789 -o /dev/null -t 1 -v 2>&1 | grep "TOTAL\|Elapsed" >> time_results.txt

four_threads_test: four_threads_test_1 four_threads_test_2 four_threads_test_3

four_threads_test_1:
	/usr/bin/time -v ./plist -l 5 -c abcdefghijklmnopqrstuvxywz -o /dev/null -t 4 -v 2>&1 | grep "TOTAL\|Elapsed" >> time_results.txt

four_threads_test_2:
	/usr/bin/time -v ./plist -l 6 -c abcdefghijklmnopqrstuvxywz -o /dev/null -t 4 -v 2>&1 | grep "TOTAL\|Elapsed" >> time_results.txt

four_threads_test_3:
	/usr/bin/time -v ./plist -l 6 -c abcdefghijklmnopqrstuvxywz0123456789 -o /dev/null -t 4 -v 2>&1 | grep "TOTAL\|Elapsed" >> time_results.txt

