snakegl: main.o init.o play.o
	gcc -o snakegl main.o init.o play.o -lGL -lGLU -lGLEW -lglut -lm

main.o: main.c snakegl.h
	gcc -c main.c

init.o: init.c snakegl.h
	gcc -c init.c

play.o: play.c snakegl.h
	gcc -c play.c
