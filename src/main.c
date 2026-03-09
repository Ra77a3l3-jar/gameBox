#include <raylib.h>
#include "game.h"
#include "utils.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(120);

    InitAudioDevice();

    SetExitKey(KEY_NULL); // Esc key can be reused

    GameState state = {0};
    InitGame(&state);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        UpdateGame(&state);
        DrawGame(&state);

        EndDrawing();
    }

    CloseGame(&state);
    CloseAudioDevice();
    CloseWindow();
}
