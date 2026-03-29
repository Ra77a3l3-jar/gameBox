#ifndef GAMEBOX_H
#define GAMEBOX_H

#include <raylib.h>
#include <stdbool.h>

typedef void (*GameInitFunc)(void*);
typedef bool (*GameUpdateFunc)(void*); // False to return to home
typedef void (*GameDrawFunc)(void*);
typedef void (*GameCloseFunc)(void*);

typedef enum {
    GAMEBOX_MENU,
    GAMEBOX_PONG,
    GAMEBOX_BREAKOUT
} GameBoxScreen;

typedef struct {
    GameBoxScreen current_screen;
    GameBoxScreen prev_screen;
    
    int selected_game; // 0 for PONG, 1 for BREAKOUT

    void *game_state; // Current game

    GameInitFunc game_init;
    GameUpdateFunc game_update;
    GameDrawFunc game_draw;
    GameCloseFunc game_close;
} GameBoxState;

void InitGameBox(GameBoxState *state);
void UpdateGameBox(GameBoxState *state);
void DrawGameBox(GameBoxState *state);
void CloseGameBox(GameBoxState *state);

void RegisterPongGame(GameBoxState *state);
void RegisterBreakoutGame(GameBoxState *state);

#endif
