#include <stdio.h>
#include "game.h"


int main(){
    int option;

    do{
        printf("\n=== WORDLE GAME ===\n");
        printf("1. Start game\n");
        printf("2. Instructions\n");
        printf("0. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &option);

        while(getchar() != '\n');

        switch(option){
            case 1:
                play_game();
                break;
            case 2:
                show_instructions();
                break;
            case 0:
                printf("Exiting game...\n");
                break;
            default:
                printf("Invalid option. Try again\n");
        }
    }

    while(option != 0);

    return 0;
}