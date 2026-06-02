#include "raylib.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "words.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 900

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6

#define TILE_SIZE 70
#define TILE_SPACING 10

typedef enum{
    SCREEN_CATEGORY,
    SCREEN_DIFFICULTY,
    SCREEN_GAME
}ScreenState;

static const char *category_names[] = {
    "Animals",
    "Objects",
    "Technology"
};

static const char *difficulty_names[] = {
    "Easy",
    "Medium",
    "Hard"
};

static const char *word_files[3][3] = {
    {
        "data/animals_easy.txt",
        "data/animals_medium.txt",
        "data/animals_hard.txt"
    },
    {
        "data/objects_easy.txt",
        "data/objects_medium.txt",
        "data/objects_hard.txt"
    },
    {
        "data/technology_easy.txt",
        "data/technology_medium.txt",
        "data/technology_hard.txt"
    }
};

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

static int calculate_score(int attempts_used){
    return (MAX_ATTEMPTS - attempts_used + 1) * 100;
}

static void handle_input(char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1], char feedbacks[MAX_ATTEMPTS][WORD_LENGTH + 1], int *current_row, int *current_col, const char secret_word[], char words[][MAX_LINE_LENGTH], int word_count, int *game_over, int *won, int *score, char message[]){
   
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
                *score = calculate_score(*current_row + 1);
            }

            (*current_row)++;
            *current_col = 0;

            if(*current_row == MAX_ATTEMPTS && !(*won)){
                *game_over = 1;
            }
        }
    }
}

static void reset_game(char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1], char feedbacks[MAX_ATTEMPTS][WORD_LENGTH + 1], char words[][MAX_LINE_LENGTH], int word_count, char secret_word[], int *current_row, int *current_col, int *game_over, int *won, int *score, int *high_score_saved, char message[]){
    for(int i = 0; i < MAX_ATTEMPTS; i++){
        for(int j = 0; j <= WORD_LENGTH; j++){
            guesses[i][j] = '\0';
            feedbacks[i][j] = '\0';
        }
    }

    choose_random_word(words, word_count, secret_word);

    *current_row = 0;
    *current_col = 0;
    *game_over = 0;
    *won = 0;
    *score = 0;
    *high_score_saved = 0;
    message[0] = '\0';
}

static void clear_char_queue(void){
    while(GetCharPressed() > 0){

    }
}

static int load_high_score(void){
    FILE *f = fopen("highscore.txt", "r");
    int high_score = 0;

    if(f == NULL){
        return 0;
    }

    fscanf(f, "%d", &high_score);
    fclose(f);

    return high_score;
}
static void save_high_score(int high_score){
    FILE *f = fopen("highscore.txt", "w");

    if(f == NULL){
        return;
    }

    fprintf(f, "%d", high_score);
    fclose(f);
}

static void draw_category_screen(void){
    DrawText("WORDLE", 300, 80, 50, BLACK);
    DrawText("Choose a category", 260, 180, 30, DARKGRAY);

    DrawText("1. Animals", 310, 260, 25, BLACK);
    DrawText("2. Objects", 310, 310, 25, BLACK);
    DrawText("3. Technology", 310, 360, 25, BLACK);

    DrawText("Press 1, 2 or 3", 300, 460, 20, GRAY);
}

static void draw_difficulty_screen(const char *categoty_name){
    char title[100];

    DrawText("WORDLE", 300, 80, 50, BLACK);

    snprintf(title, sizeof(title), "Category: %s", categoty_name);
    DrawText(title, 280, 160, 25, DARKGRAY);

    DrawText("Choose difficulty", 265, 220, 30, DARKGRAY);

    DrawText("1. Easy", 330, 300, 25, BLACK);
    DrawText("2. Medium", 330, 350, 25, BLACK);
    DrawText("3. Hard", 330, 400, 25, BLACK);

    DrawText("Press 1, 2 or 3", 300, 500, 20, GRAY);
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
    int block_next_input = 0;
    int score = 0;
    int high_score = load_high_score();
    int high_score_saved = 0;
    ScreenState screen = SCREEN_CATEGORY;
    int selected_category = -1;
    int selected_difficulty = -1;
    char message[100] = "";

    /*word_count = load_words("words.txt", words);

    if(word_count == 0){
        printf("No words loaded from words.txt\n");
        return 1;
    }

    choose_random_word(words, word_count, secret_word);*/


    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Wordle Game");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if(screen == SCREEN_CATEGORY){
            if(IsKeyPressed(KEY_ONE)){
                selected_category = 0;
                screen = SCREEN_DIFFICULTY;
            }
            else if(IsKeyPressed(KEY_TWO)){
                selected_category = 1;
                screen = SCREEN_DIFFICULTY;
            }
            else if(IsKeyPressed(KEY_THREE)){
                selected_category = 2;
                screen = SCREEN_DIFFICULTY;
            }
        }
        else if(screen == SCREEN_DIFFICULTY){
            if(IsKeyPressed(KEY_ONE)){
                selected_difficulty = 0;
            }
            else if(IsKeyPressed(KEY_TWO)){
                selected_difficulty = 1;
            }
            else if(IsKeyPressed(KEY_THREE)){
                selected_difficulty = 2;
            }
            if(selected_difficulty != -1){
                word_count = load_words(word_files[selected_category][selected_difficulty], words);

                if(word_count == 0){
                    strcpy(message, "Could not load words file");
                }
                else{
                    choose_random_word(words, word_count, secret_word);
                    screen = SCREEN_GAME;
                }
            }
        }
        else if(screen == SCREEN_GAME){
            if(!game_over){
                if(block_next_input){
                    clear_char_queue();
                    block_next_input = 0;
                }
                else{
                    handle_input(guesses, feedbacks, &current_row, &current_col, secret_word, words, word_count, &game_over, &won, &score, message);

                }
            }
            else{
                if(IsKeyPressed(KEY_R)){
                    reset_game(guesses, feedbacks, words, word_count, secret_word, &current_row, &current_col, &game_over, &won, &score, &high_score_saved, message);

                    block_next_input = 1;
                }
            }

            if(game_over && won && !high_score_saved){
                if(score > high_score){
                    high_score = score;
                    save_high_score(high_score);
                }

                high_score_saved = 1;
            }
        }
        

        BeginDrawing();

        ClearBackground(RAYWHITE);

        if(screen == SCREEN_CATEGORY){
            draw_category_screen();
        }
        else if(screen == SCREEN_DIFFICULTY){
            draw_difficulty_screen(category_names[selected_category]);

            if(message[0] != '\0'){
                DrawText(message, 260, 560, 22, RED);
            }
        }
        else if(screen == SCREEN_GAME){
            DrawText("WORDLE", 300, 40, 50, BLACK);

            draw_board(guesses, feedbacks, current_row);

            char score_text[100];
            snprintf(score_text, sizeof(score_text), "Score: %d", score);
            DrawText(score_text, 250, 650, 20, DARKGRAY);

            char high_score_text[100];
            snprintf(high_score_text, sizeof(high_score_text), "High Score: %d", high_score);
            DrawText(high_score_text, 390, 650, 20, DARKGRAY);

            if(!game_over){
                DrawText("Type a 5 letter word.", 300, 690, 20, GRAY);
                DrawText("ENTER = submit    BACKSPACE = delete", 220, 720, 20, GRAY);

                if(message[0] != '\0'){
                    DrawText(message, 285, 760, 22, RED);
                }
            }
            else{
                if(won){
                    DrawText("You won!", 330, 590, 30, GREEN);

                    char final_score[100];
                    snprintf(final_score, sizeof(final_score), "Final score: %d", score);
                    DrawText(final_score, 315, 730, 20, DARKGRAY);
                }
                else{
                    DrawText("You lost!", 330, 675, 30, RED);

                    char message[150];
                    snprintf(message, sizeof(message), "Word was: %s", secret_word);
                    DrawText(message, 310, 715, 20, DARKGRAY);
                }

                DrawText("Press R to restart", 300, 760, 20, DARKGRAY);
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}