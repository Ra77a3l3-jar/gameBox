#include <raylib.h>

int main(void) {
    InitWindow(1200, 720, "Pong");
    SetTargetFPS(120);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Hello Pong", 500, 350, 35, WHITE);
        EndDrawing();
    }
    CloseWindow();
}
