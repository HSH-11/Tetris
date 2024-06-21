# Set compiler to use
CC=g++
CFLAGS=-g -I. -fpermissive -Wno-deprecated
LDFLAGS=
DEBUG=0

all:: Lab1.exe Lab2.exe

Lab1.exe: Lab1.o Matrix.o
	$(CC) -o $@ $^ -std=c++11 $(CFLAGS) $(LDFLAGS)

Lab2.exe: Lab2.o Matrix_e.o
	$(CC) -o $@ $^ -std=c++11 $(CFLAGS) $(LDFLAGS)

%.o: %.c $(DEPS_CHAT)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS_TET)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f *.exe 
	rm -f *.o *~ *.stackdump
