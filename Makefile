CC := gcc --std=c99 -lm -lSDL -Wall -Werror

all: main

perlin.o: perlin.c
	${CC} -c -o perlin.o perlin.c

main.o: main.c
	${CC} -c -o main.o main.c

main: perlin.o main.o
	${CC} -o main perlin.o main.o

test: main
	./main
