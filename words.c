#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "words.h"


int load_words(const char *filename, char words[][MAX_LINE_LENGTH]){
    FILE *f;
    int count = 0;

    f = fopen(filename, "r");
    if(f == NULL){
        printf("Error opening file %s\n", filename);
        return 0;
    }

    while(fgets(words[count], MAX_LINE_LENGTH, f) != NULL && count < MAX_WORDS){
        words[count][strcspn(words[count], "\n")] = '\0';
        count++;
    }

    fclose(f);
    return count;
}

void choose_random_word(char words[][MAX_LINE_LENGTH], int count, char secret_word[]){
    int index;

    if(count == 0){
        secret_word[0] = '\0';
        return;
    }

    srand(time(NULL));
    index = rand() % count;
    strcpy(secret_word, words[index]);
}

int is_valid_word(const char *word, char words[][MAX_LINE_LENGTH], int count){
    int i;

    for(i = 0; i < count; i++){
        if(strcmp(word, words[i]) == 0){
            return 1;
        }
    }

    return 0;
}