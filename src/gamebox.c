#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gamebox.h"
#include "pong.h"
#include "breakout.h"

void InitGameBox(GameBoxState *state) {
    state->current_screen = GAMEBOX_MENU;
    state->prev_screen = GAMEBOX_MENU;

    state->game_state = NULL;
    state->game_init = NULL;
    state->game_update = NULL;
    state->game_draw = NULL;
    state->game_close = NULL;
}

void UpdateGameBox(GameBoxState *state) {
    if(state->current_screen == GAMEBOX_MENU) {
        if(IsKeyPressed(KEY_ONE)) {
            RegisterPongGame(state);
            state->current_screen = GAMEBOX_PONG;
            state->prev_screen = GAMEBOX_MENU;
        } else if(IsKeyPressed(KEY_TWO)) {
            RegisterBreakoutGame(state);
            state->current_screen = GAMEBOX_BREAKOUT;
            state->prev_screen = GAMEBOX_MENU;
        }

        if(state->game_init) {
            state->game_init(state->game_state);
        }
    } else {
        if(state->game_update) {
            bool continue_game = state->game_update(state->game_state);
            if(!continue_game) {
                if(state->game_close) {
                    state->game_close(state->game_state);
                }
                free(state->game_state);
                state->game_state = NULL;
                state->current_screen = GAMEBOX_MENU;
                return;
            }
        }
    }
}

void DrawGameBox(GameBoxState *state) {
    if(state->current_screen == GAMEBOX_MENU) {
        ClearBackground(BLACK);

        DrawText("GAMEBOX", GetScreenWidth()/2 - MeasureText("GAMEBOX", 80)/2, 100, 80, ORANGE);

        // Game selection
        DrawText("1. PONG", GetScreenWidth()/2 - MeasureText("1. PONG", 50)/2, 300, 40, WHITE);
        DrawText("2. BREAKOUT", GetScreenWidth()/2 - MeasureText("2. BREAKOUT", 50)/2, 360, 40, WHITE);
        DrawText("Press 1 or 2 to play", GetScreenWidth()/2 - MeasureText("Press 1 or 2 to play", 25)/2, 400, 25, GRAY);
        DrawText("Press ESC to exit", GetScreenWidth()/2 - MeasureText("Press ESC to exit", 20)/2, 450, 20, GRAY);
    } else {
        // Draw current game
        if(state->game_draw) {
            state->game_draw(state->game_state);
        }
    }
}

void CloseGameBox(GameBoxState *state) {
    if(state->game_close && state->game_state) {
        state->game_close(state->game_state);
        free(state->game_state);
    }
}

void RegisterPongGame(GameBoxState *state) {
    state->game_state = malloc(sizeof(PongGameState));

    state->game_init = (GameInitFunc)PongInit;
    state->game_update = (GameUpdateFunc)PongUpdate;
    state->game_draw = (GameDrawFunc)PongDraw;
    state->game_close = (GameCloseFunc)PongClose;
}

void RegisterBreakoutGame(GameBoxState *state) {
    state->game_state = malloc(sizeof(BreakoutGameState));

    state->game_init = (GameInitFunc)BreakoutInit;
    state->game_update = (GameUpdateFunc)BreakoutUpdate;
    state->game_draw = (GameDrawFunc)BreakoutDraw;
    state->game_close = (GameCloseFunc)BreakoutClose;
}
