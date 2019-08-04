CFLAGS= -Wall -fpic -coverage -lm -std=c99

testme: testme.c
	gcc -o testme -g testme.c $(CFLAGS)

randomtest: testme
	-./testme
	gcov testme.c -b
	cat testme.c.gcov

clean:
	rm -rf *.o *.gcno *.gcda testme idir
