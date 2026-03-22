#ifndef GAMEBOX_H
#define GAMEBOX_H

#include <raylib.h>
#include <stdbool.h>

typedef enum {
    GAMEBOX_MENU,
    GAMEBOX_PONG
} GameBoxScreen;

typedef struct {
    GameBoxScreen current_screen;
    GameBoxScreen prev_screen;

    void *game_state; // Current game

    void (*game_init)(void *);
    void (*game_update)(void *);
    void (*game_draw)(void *);
    void (*game_close)(void *);
} GameBoxState;

void InitGameBox(GameBoxState *state);
void UpdateGameBox(GameBoxState *state);
void DrawGameBox(GameBoxState *state);
void CloseGameBox(GameBoxState *state);

#endif
