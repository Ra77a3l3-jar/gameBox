#include <raylib.h>
#include "gamebox.h"
#include "utils.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GAMEBOX");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(120);

    InitAudioDevice();

    SetExitKey(KEY_NULL); // Esc key can be reused

    GameBoxState state = {0};
    InitGameBox(&state);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        UpdateGameBox(&state);
        DrawGameBox(&state);

        EndDrawing();
    }

    CloseGameBox(&state);
    CloseAudioDevice();
    CloseWindow();
}
