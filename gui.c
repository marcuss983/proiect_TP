#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 700

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6

#define TILE_SIZE 70
#define TILE_SPACING 10

static void draw_board(void)
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
        }
    }
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Wordle Game");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("WORDLE", 300, 40, 50, BLACK);

        draw_board();

        DrawText("Graphical version - first step", 250, 620, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}