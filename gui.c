#include "raylib.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "words.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 700

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6

#define TILE_SIZE 70
#define TILE_SPACING 10


static void to_lowercase(char text[]){
    for(int i = 0; text[i] != '\0'; i++){
        text[i] = (char)tolower(text[i]);
    }
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

static Color get_tile_color(char feedback){
    if(feedback == 'G'){
        return GREEN;
    }

    if(feedback == 'Y'){
        return GOLD;
    }

    if(feedback == 'B'){
        return GRAY;
    }

    return RAYWHITE;
}

static void draw_board(char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1], char feedbacks[MAX_ATTEMPTS][WORD_LENGTH + 1], int current_row)
{
    int row;
    int col;

    int board_width = WORD_LENGTH * TILE_SIZE + (WORD_LENGTH - 1) * TILE_SPACING;
    int start_x = (SCREEN_WIDTH - board_width) / 2;
    int start_y = 120;

    for (row = 0; row < MAX_ATTEMPTS; row++)
    {
        for (col = 0; col < WORD_LENGTH; col++)
        {
            int x = start_x + col * (TILE_SIZE + TILE_SPACING);
            int y = start_y + row * (TILE_SIZE + TILE_SPACING);

            Color tile_color = RAYWHITE;
            Color text_color = BLACK;

            if(row < current_row){
                tile_color = get_tile_color(feedbacks[row][col]);
                text_color = WHITE;
            }

            DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, tile_color);
            DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, DARKGRAY);

            if(guesses[row][col] != '\0'){
                char letter[2];
                letter[0] = guesses[row][col];
                letter[1] = '\0';

                DrawText(letter, x + 24, y + 15, 40, text_color);
            }
        }
    }
}

static void handle_input(char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1], char feedbacks[MAX_ATTEMPTS][WORD_LENGTH + 1], int *current_row, int *current_col, const char secret_word[], char words[][MAX_LINE_LENGTH], int word_count, int *game_over, int *won, char message[]){
   
    int key = GetCharPressed();

    while(key > 0){
        if(isalpha(key) && *current_col < WORD_LENGTH && *current_row < MAX_ATTEMPTS){
            message[0] = '\0';
            guesses[*current_row][*current_col] = (char)toupper(key);
            (*current_col)++;
        }

        key = GetCharPressed();
    }

    if(IsKeyPressed(KEY_BACKSPACE)){
        if(*current_col > 0){
            message[0] = '\0';
            (*current_col)--;
            guesses[*current_row][*current_col] = '\0';
        }
    }

    if(IsKeyPressed(KEY_ENTER)){
        if(*current_col < WORD_LENGTH){
            strcpy(message, "Not enough letters");
            return;
        }

        if(*current_col == WORD_LENGTH){
            char guess_lower[WORD_LENGTH + 1];

            strcpy(guess_lower, guesses[*current_row]);
            to_lowercase(guess_lower);

            if(!is_valid_word(guess_lower, words, word_count)){
                strcpy(message, "Word not in dictionary");
                return;
            }

            message[0] = '\0';

            check_guess(secret_word, guess_lower, feedbacks[*current_row]);

            if(strcmp(guess_lower, secret_word) == 0){
                *won = 1;
                *game_over = 1;
            }

            (*current_row++);
            *current_col = 0;

            if(*current_row == MAX_ATTEMPTS && !(*won)){
                *game_over = 1;
            }
        }
    }
}

int main(void)
{
    char words[MAX_WORDS][MAX_LINE_LENGTH];
    char secret_word[MAX_LINE_LENGTH];

    char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1] = {0};
    char feedbacks[MAX_ATTEMPTS][WORD_LENGTH + 1] = {0};
    
    int word_count;
    int current_row = 0;
    int current_col = 0;
    int game_over = 0;
    int won = 0;
    char message[100] = "";

    word_count = load_words("words.txt", words);

    if(word_count == 0){
        printf("No words loaded from words.txt\n");
        return 1;
    }

    choose_random_word(words, word_count, secret_word);


    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Wordle Game");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if(!game_over){
            handle_input(guesses, feedbacks, &current_row, &current_col, secret_word, words, word_count, &game_over, &won, message);
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("WORDLE", 300, 40, 50, BLACK);

        draw_board(guesses, feedbacks, current_row);

        if(!game_over){
            DrawText("Type a 5 letter word. ENTER to submit. BACKSPACE to delete.", 95, 620, 20, GRAY);

            if(message[0] != '\0'){
                DrawText(message, 285, 640, 22, RED);
            }
        }
        else{
            if(won){
                DrawText("You won!", 330, 610, 30, GREEN);
            }
            else{
                DrawText("You lost!", 330, 590, 30, RED);

                char message[150];
                snprintf(message, sizeof(message), "Word was: %s", secret_word);
                DrawText(message, 310, 630, 20, DARKGRAY);
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}