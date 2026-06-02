#include "raylib.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "words.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 900

#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 6
#define MAX_ATTEMPTS 6

#define TILE_SIZE 70
#define TILE_SPACING 10


typedef enum{
    SCREEN_MODE_MENU,
    SCREEN_CATEGORY,
    SCREEN_DIFFICULTY,
    SCREEN_MULTIPLAYER_SECRET,
    SCREEN_GAME
}ScreenState;

static const char *category_names[] = {
    "Animals",
    "Objects",
    "Technology",
    "Football Teams"
};

static const char *difficulty_names[] = {
    "Easy",
    "Medium",
    "Hard"
};

static const char *word_files[4][3] = {
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
    },
    {
        "data/football_easy.txt",
        "data/football_medium.txt",
        "data/football_hard.txt"
    }
};

static const char *allowed_files[3] = {
    "data/allowed_4.txt",
    "data/allowed_5.txt",
    "data/allowed_6.txt"
};

static void to_lowercase(char text[]){
    for(int i = 0; text[i] != '\0'; i++){
        text[i] = (char)tolower(text[i]);
    }
}

static void check_guess(const char secret_word[], const char guess[], char feedback[], int word_length){
    int used_secret[MAX_WORD_LENGTH] = {0};

    for(int i = 0; i < word_length; i++){
        feedback[i] = 'B';
    }

    for(int i = 0; i < word_length; i++){
        if(guess[i] == secret_word[i]){
            feedback[i] = 'G';
            used_secret[i] = 1;
        }
    }

    for(int i = 0; i < word_length; i++){
        if(feedback[i] == 'G'){
            continue;
        }

        for(int j = 0; j < word_length; j++){
            if(!used_secret[j] && guess[i] == secret_word[j]){
                feedback[i] = 'Y';
                used_secret[j] = 1;
                break;
            }
        }
    }

    feedback[word_length] = '\0';
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

static void draw_board(char guesses[MAX_ATTEMPTS][MAX_WORD_LENGTH + 1], char feedbacks[MAX_ATTEMPTS][MAX_WORD_LENGTH + 1], int current_row, int word_length)
{
    int row;
    int col;

    int board_width = word_length * TILE_SIZE + (word_length - 1) * TILE_SPACING;
    int start_x = (SCREEN_WIDTH - board_width) / 2;
    int start_y = 120;

    for (row = 0; row < MAX_ATTEMPTS; row++)
    {
        for (col = 0; col < word_length; col++)
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

static void handle_input(char guesses[MAX_ATTEMPTS][MAX_WORD_LENGTH + 1], char feedbacks[MAX_ATTEMPTS][MAX_WORD_LENGTH + 1], int *current_row, int *current_col, const char secret_word[], char allowed_words[][MAX_LINE_LENGTH], int allowed_count, int *game_over, int *won, int *score, int word_length, char message[]){
   
    int key = GetCharPressed();

    while(key > 0){
        if(isalpha(key) && *current_col < word_length && *current_row < MAX_ATTEMPTS){
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
        if(*current_col < word_length){
            strcpy(message, "Not enough letters");
            return;
        }

        if(*current_col == word_length){
            char guess_lower[MAX_WORD_LENGTH + 1];

            strcpy(guess_lower, guesses[*current_row]);
            to_lowercase(guess_lower);

            if(!is_valid_word(guess_lower, allowed_words, allowed_count)){
                strcpy(message, "Word not in dictionary");
                return;
            }

            message[0] = '\0';

            check_guess(secret_word, guess_lower, feedbacks[*current_row], word_length);

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

static void reset_game(char guesses[MAX_ATTEMPTS][MAX_WORD_LENGTH + 1], char feedbacks[MAX_ATTEMPTS][MAX_WORD_LENGTH + 1], char secret_word[], int *current_row, int *current_col, int *game_over, int *won, int *score, int *high_score_saved, char message[]){
    for(int i = 0; i < MAX_ATTEMPTS; i++){
        for(int j = 0; j <= MAX_WORD_LENGTH; j++){
            guesses[i][j] = '\0';
            feedbacks[i][j] = '\0';
        }
    }

    secret_word[0] = '\0';

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
    DrawText("4. Football Teams", 310, 410, 25, BLACK);

    DrawText("Press 1, 2, 3 or 4", 285, 510, 20, GRAY);
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

static void draw_mode_menu(void){
    DrawText("WORDLE", 300, 100, 50, BLACK);

    DrawText("Choose game mode", 260, 200, 30, DARKGRAY);

    DrawText("1. Single Player", 290, 280, 25, BLACK);
    DrawText("2. Local Multiplayer", 290, 330, 25, BLACK);

    DrawText("Press 1 or 2", 320, 430, 20, GRAY);
}

static void handle_secret_word_input(char secret_word[], int *secret_col, ScreenState *screen, char answer_words[][MAX_LINE_LENGTH], int answer_count, int word_length, char message[]){
    int key = GetCharPressed();

    while(key > 0){
        if(isalpha(key) && *secret_col < word_length){
            secret_word[*secret_col] = (char)tolower(key);
            (*secret_col)++;
            message[0] = '\0';
        }

        key = GetCharPressed();
    }

    if(IsKeyPressed(KEY_BACKSPACE)){
        if(*secret_col > 0){
            (*secret_col)--;
            secret_word[*secret_col] = '\0';
            message[0] = '\0';
        }
    }

    if(IsKeyPressed(KEY_ENTER)){
        if(*secret_col < word_length){
            snprintf(message, 100, "Secret word must have %d letters", word_length);
            return;
        }

        secret_word[word_length] = '\0';

        if(!is_valid_word(secret_word, answer_words, answer_count)){
            strcpy(message, "Secret word is not in dictionary");
            return;
        }
        message[0] = '\0';
        *screen = SCREEN_GAME;
    }
}

static void draw_secret_word_screen(const char secret_word[], int secret_col, int word_length, const char message[]){
    DrawText("LOCAL MULTIPLAYER", 210, 100, 40, BLACK);

    DrawText("Player 1: Enter the secret word", 210, 200, 25, DARKGRAY);
    DrawText("The word will be hidden", 270, 235, 20, GRAY);

    int start_x = 290;
    int y = 320;

    for(int i = 0; i < word_length; i++){
        DrawRectangleLines(start_x + i * 45, y, 35, 45, DARKGRAY);

        if(i < secret_col){
            DrawText("*", start_x + i * 45 + 10, y + 5, 35, BLACK);
        }
    }

    DrawText("ENTER = confirm     BACKSPACE = delete", 210, 420, 20, GRAY);

    if(message[0] != '\0'){
        DrawText(message, 240, 470, 22, RED);
    }
}

static int get_word_length_by_difficulty(int difficulty){
    if(difficulty == 0){
        return 4;
    }

    if(difficulty == 1){
        return 5;
    }
    return 6;
}


int main(void)
{
    static char answer_words[MAX_WORDS][MAX_LINE_LENGTH];
    static char allowed_words[MAX_WORDS][MAX_LINE_LENGTH];

    char secret_word[MAX_LINE_LENGTH];

    char guesses[MAX_ATTEMPTS][MAX_WORD_LENGTH + 1] = {0};
    char feedbacks[MAX_ATTEMPTS][MAX_WORD_LENGTH + 1] = {0};
    
    int answer_count = 0;
    int allowed_count = 0;
    int current_row = 0;
    int current_col = 0;
    int game_over = 0;
    int won = 0;
    int block_next_input = 0;
    int score = 0;
    int high_score = load_high_score();
    int high_score_saved = 0;
    ScreenState screen = SCREEN_MODE_MENU;
    int selected_category = -1;
    int selected_difficulty = -1;
    int secret_col = 0;
    int multiplayer = 0;
    int word_length = 5;
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
        if(screen == SCREEN_MODE_MENU){
            if(IsKeyPressed(KEY_ONE)){
                multiplayer = 0;
                selected_category = -1;
                selected_difficulty = -1;
                message[0] = '\0';

                screen = SCREEN_CATEGORY;
                block_next_input = 1;
            }
            else if(IsKeyPressed(KEY_TWO)){
                multiplayer = 1;
                selected_category = -1;
                selected_difficulty = -1;
                message[0] = '\0';

                screen = SCREEN_CATEGORY;
                block_next_input = 1;
            }
        }
        else if(screen == SCREEN_CATEGORY){
            if(block_next_input){
                clear_char_queue();
                block_next_input = 0;
            }
            else if(IsKeyPressed(KEY_ONE)){
                selected_category = 0;
                selected_difficulty = -1;
                message[0] = '\0';
                screen = SCREEN_DIFFICULTY;
                block_next_input = 1;
            }
            else if(IsKeyPressed(KEY_TWO)){
                selected_category = 1;
                selected_difficulty = -1;
                message[0] = '\0';
                screen = SCREEN_DIFFICULTY;
                block_next_input = 1;
            }
            else if(IsKeyPressed(KEY_THREE)){
                selected_category = 2;
                selected_difficulty = -1;
                message[0] = '\0';
                screen = SCREEN_DIFFICULTY;
                block_next_input = 1;
            }
            else if(IsKeyPressed(KEY_FOUR)){
                selected_category = 3;
                selected_difficulty = -1;
                message[0] = '\0';
                screen = SCREEN_DIFFICULTY;
                block_next_input = 1;
            }
        }
        else if(screen == SCREEN_DIFFICULTY){
            if(block_next_input){
                clear_char_queue();
                block_next_input = 0;
            }
            else{
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
                    word_length = get_word_length_by_difficulty(selected_difficulty);
                    answer_count = load_words(word_files[selected_category][selected_difficulty], answer_words);
                    allowed_count = load_words(allowed_files[selected_difficulty], allowed_words);

                    if(answer_count == 0 || allowed_count == 0){
                        strcpy(message, "Could not load words file");
                        selected_difficulty = -1;
                    }
                    else{
                        reset_game(guesses, feedbacks, secret_word, &current_row, &current_col, &game_over, &won, &score, &high_score_saved, message);

                        if(multiplayer){
                            secret_word[0] = '\0';
                            secret_col = 0;
                            screen = SCREEN_MULTIPLAYER_SECRET;
                        }
                        else{
                            choose_random_word(answer_words, answer_count, secret_word);

                            screen = SCREEN_GAME;
                        }
                        
                        block_next_input = 1;
                    }
                }
            }
        }
        else if(screen == SCREEN_MULTIPLAYER_SECRET){
            if(block_next_input){
                clear_char_queue();
                block_next_input = 0;
            }
            else{
                handle_secret_word_input(secret_word, &secret_col, &screen, answer_words, answer_count, word_length, message);
            }
        }
        else if(screen == SCREEN_GAME){
            if(!game_over){
                if(block_next_input){
                    clear_char_queue();
                    block_next_input = 0;
                }
                else{
                    handle_input(guesses, feedbacks, &current_row, &current_col, secret_word, allowed_words, allowed_count, &game_over, &won, &score, word_length, message);
                }
            }
            else{
                if(IsKeyPressed(KEY_R)){
                    reset_game(guesses, feedbacks, secret_word, &current_row, &current_col, &game_over, &won, &score, &high_score_saved, message);

                    if(multiplayer){
                        secret_word[0] = '\0';
                        secret_col = 0;
                        screen = SCREEN_MULTIPLAYER_SECRET;
                    }
                    else{
                        choose_random_word(answer_words, answer_count, secret_word);
                        screen = SCREEN_GAME;
                    }
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

        if(screen == SCREEN_MODE_MENU){
            draw_mode_menu();

            if(message[0] != '\0'){
                DrawText(message, 270, 500, 22, RED);
            }
        }
        else if(screen == SCREEN_CATEGORY){
            draw_category_screen();
        }
        else if(screen == SCREEN_DIFFICULTY){
            draw_difficulty_screen(category_names[selected_category]);

            if(message[0] != '\0'){
                DrawText(message, 260, 560, 22, RED);
            }
        }

        else if(screen ==  SCREEN_MULTIPLAYER_SECRET){
            draw_secret_word_screen(secret_word, secret_col, word_length, message);
        }
        else if(screen == SCREEN_GAME){
            DrawText("WORDLE", 300, 40, 50, BLACK);

            if(multiplayer){
                DrawText("Local Multiplayer", 300, 95, 20, DARKGRAY);
            }
            else{
                DrawText("Single Player", 330, 95, 20, DARKGRAY);
            }

            draw_board(guesses, feedbacks, current_row, word_length);

            char score_text[100];
            snprintf(score_text, sizeof(score_text), "Score: %d", score);
            DrawText(score_text, 250, 650, 20, DARKGRAY);

            char high_score_text[100];
            snprintf(high_score_text, sizeof(high_score_text), "High Score: %d", high_score);
            DrawText(high_score_text, 390, 650, 20, DARKGRAY);

            if(!game_over){
                char instruction[100];
                snprintf(instruction, sizeof(instruction), "Type a %d letter word", word_length);
                DrawText(instruction, 300, 690, 20, GRAY);
                DrawText("ENTER = submit     BACKSPACE = delete", 220, 720, 20, GRAY);

                if(message[0] != '\0'){
                    DrawText(message, 285, 760, 22, RED);
                }
            }
            else{
                if(won){
                    DrawText("You won!", 330, 690, 30, GREEN);

                    char final_score[100];
                    snprintf(final_score, sizeof(final_score), "Final score: %d", score);
                    DrawText(final_score, 315, 730, 20, DARKGRAY);
                }
                else{
                    DrawText("You lost!", 330, 675, 30, RED);

                    char final_message[150];
                    snprintf(final_message, sizeof(final_message), "Word was: %s", secret_word);
                    DrawText(final_message, 310, 715, 20, DARKGRAY);
                }

                if(multiplayer){
                    DrawText("Press R for new secret word", 260, 760, 20, DARKGRAY);
                }
                else{
                    DrawText("Press R to restart", 300, 760, 20, DARKGRAY);
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}