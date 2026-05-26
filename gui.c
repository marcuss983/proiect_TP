#include "raylib.h"
#include <ctype.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 700

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6

#define TILE_SIZE 70
#define TILE_SPACING 10

static void draw_board(char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1])
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

            DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, DARKGRAY);

            if(guesses[row][col] != '\0'){
                char letter[2];
                letter[0] = guesses[row][col];
                letter[1] = '\0';

                DrawText(letter, x + 25, y + 15, 40, BLACK);
            }
        }
    }
}

static void handle_input(char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1], int *current_row, int *current_col){
    int key = GetCharPressed();

    while(key > 0){
        if(isalpha(key) && *current_col < WORD_LENGTH && *current_row < MAX_ATTEMPTS){
            guesses[*current_row][*current_col] = (char)toupper(key);
            (*current_col)++;
        }

        key = GetCharPressed();
    }

    if(IsKeyPressed(KEY_BACKSPACE)){
        if(*current_col > 0){
            (*current_col)--;
            guesses[*current_row][*current_col] = '\0';
        }
    }

    if(IsKeyPressed(KEY_ENTER)){
        if(*current_col == WORD_LENGTH){
            (*current_row)++;
            *current_col = 0;
        }
    }
}

int main(void)
{
    char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1] = {0};
    int current_row = 0;
    int current_col = 0;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Wordle Game");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if(current_row < MAX_ATTEMPTS){
            handle_input(guesses, &current_row, &current_col);
        }
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("WORDLE", 300, 40, 50, BLACK);

        draw_board(guesses);

        DrawText("Type letters. Press ENTER after 5 letters. BACKSPACE deletes.", 120, 620, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}