CC=g++
CFLAGS=-O3
all:	run
run:	main.o visuals.o visuals.h colors.h
	$(CC) $(CFLAGS) -o run main.o visuals.o -lglut -lGLU -lGL -lGLEW -w
main.o:	main.cpp
	$(CC) $(CFLAGS) -c main.cpp
visuals.o: visuals.cpp
	$(CC) $(CFLAGS) -c visuals.cpp

clean:
	rm -f run main.o visuals.o
