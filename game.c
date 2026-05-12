#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "game.h"
#include "words.h"


static void remove_newline(char text[]){
    text[strcspn(text, "\n")] = '\0';
}

static void to_lowercase(char text[]){
    for(int i = 0; text[i] != '\0'; i++){
        text[i] = tolower(text[i]);
    }
}

static int is_alpha_string(const char text[]){
    for(int i = 0; text[i] != '\0'; i++){
        if(!isalpha(text[i])){
            return 0;
        }
    }

    return 1;
}

static int is_valid_guess_format(const char guess[]){
    if(strlen(guess) != WORD_LENGTH){
        return 0;
    }

    if(!is_alpha_string(guess)){
        return 0;
    }
    return 1;
}

static void check_guess(const char secret_word[], const char guess[], char feedback[]){
    int used_secret[WORD_LENGTH] = {0};

    for(int i = 0; i < WORD_LENGTH; i++){
        feedback[i] = 'B';
    }

    for(int i = 0; i < WORD_LENGTH; i++){
        if(guess[i] == secret_word[i]){
            feedback[i] = 'G';
            used_secret[i] = 1;
        }
    }

    for(int i = 0; i < WORD_LENGTH; i++){
        if(feedback[i] == 'G'){
            continue;
        }

        for(int j = 0; j < WORD_LENGTH; j++){
            if(!used_secret[j] && guess[i] == secret_word[j]){
                feedback[i] = 'Y';
                used_secret[j] = 1;
                break;
            }
        }
    }

    feedback[WORD_LENGTH] = '\0';
}

/*static void display_feedback(const char guess[], const char feedback[]){
    printf("Result: ");

    for(int i = 0; i < WORD_LENGTH; i++){
        printf("%c(%c) ", guess[i], feedback[i]);
    }

    printf("\n");
}*/

static void display_board(char guesses[][MAX_LINE_LENGTH], char feedbacks[][WORD_LENGTH + 1], int attempts){
    printf("\n=== BOARD ===\n");

    for(int i = 0; i < attempts; i++){
        printf("%s  ", guesses[i]);

        for(int j = 0; j < WORD_LENGTH; j++){
            printf("%c ", feedbacks[i][j]);
        }

        printf("\n");
    }
}

void play_game(void){
    char words[MAX_WORDS][MAX_LINE_LENGTH];
    char secret_word[MAX_LINE_LENGTH];
    char guess[MAX_LINE_LENGTH];
    char feedback[WORD_LENGTH + 1];
    char guesses[MAX_ATTEMPTS][MAX_LINE_LENGTH];
    char feedbacks[MAX_ATTEMPTS][WORD_LENGTH + 1];
    int word_count;
    int attempts = 0;
    int won = 0;

    word_count = load_words("words.txt", words);

    if(word_count == 0){
        printf("No words loaded. Cannot start game\n");
        return;
    }

    choose_random_word(words, word_count, secret_word);

    while(attempts < MAX_ATTEMPTS && !won){
        printf("\nAttempt %d/%d\n", attempts + 1, MAX_ATTEMPTS);
        printf("Enter a %d-letter word: ", WORD_LENGTH);

        fgets(guess, MAX_LINE_LENGTH, stdin);
        remove_newline(guess);
        to_lowercase(guess);

        if(!is_valid_guess_format(guess)){
            printf("Invalid input. The word must have exactly %d letters and contain only alphabetic characters\n", WORD_LENGTH);
            continue;
        }

        if(!is_valid_word(guess, words, word_count)){
            printf("Invalid word. The word is not in the dictionary\n");
            continue;
        }

        check_guess(secret_word, guess, feedback);

        strcpy(guesses[attempts], guess);
        strcpy(feedbacks[attempts], feedback);

        attempts++;

        display_board(guesses, feedbacks, attempts);

        if(strcmp(guess, secret_word) == 0){
            won = 1;
        }
    }

    if(won){
        printf("\nYou won! The word was: %s\n", secret_word);
    }
    else{
        printf("\nYou lost! The word was: %s\n", secret_word);
    }
}


void show_instructions(void){
    printf("\n=== INSTRUCTIONS ===\n");
    printf("You have to guess the hidden 5-letter word\n");
    printf("You have %d attempts\n", MAX_ATTEMPTS);
    printf("After each guess, the game will give feedback for each letter\n");
    printf("G = correct letter and correct position\n");
    printf("Y = letter exists in the word, but is in the wrong position\n");
    printf("B = letter does not exist in the word\n");
}