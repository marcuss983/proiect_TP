#include <stdio.h>
#include "game.h"


void play_game(void){
    printf("\nGame will be implemented step by step\n");
}

void show_instructions(void){
    printf("\n=== INSTRUCTIONS ===\n");
    printf("You have to guess the hidden 5-letter word\n");
    printf("You have %d attempts\n", MAX_ATTEMPTS);
    printf("After each guess, the game will give feedback for each letter\n");
}