#include <raylib.h>
#include "game.h"
#include "utils.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    SetTargetFPS(120);

    GameState state = {0};
    InitGame(&state);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        UpdateGame(&state);
        DrawGame(&state);
        
        DrawText("Hello Pong", 500, 350, 35, WHITE);
        EndDrawing();
    }
    CloseGame(&state);
    CloseWindow();
}
