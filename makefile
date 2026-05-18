CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJ = main.o game.o words.o

wordle: $(OBJ)
	$(CC) $(CFLAGS) -o wordle $(OBJ)

main.o: main.c game.h
	$(CC) $(CFLAGS) -c main.c

game.o: game.c game.h
	$(CC) $(CFLAGS) -c game.c

words.o: words.c words.h
	$(CC) $(CFLAGS) -c words.c

clean:
	rm -f *.o wordle
	
gui: gui.c
	gcc gui.c -o wordle_gui -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
