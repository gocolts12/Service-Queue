gcc -c sq.o: sq.h sq.c
	gcc -c sq.c

driver: driver.c sq.o 
	gcc driver.c sq.o -lc -o driver

