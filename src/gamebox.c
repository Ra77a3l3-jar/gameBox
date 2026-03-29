#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gamebox.h"
#include "pong.h"
#include "breakout.h"

void InitGameBox(GameBoxState *state) {
    state->current_screen = GAMEBOX_MENU;
    state->prev_screen = GAMEBOX_MENU;
    state->selected_game = 0;

    state->game_state = NULL;
    state->game_init = NULL;
    state->game_update = NULL;
    state->game_draw = NULL;
    state->game_close = NULL;
}

void UpdateGameBox(GameBoxState *state) {
    if(state->current_screen == GAMEBOX_MENU) {
        // Handle arrow key navigation
        if(IsKeyPressed(KEY_UP)) {
            state->selected_game = (state->selected_game - 1 + 2) % 2;
        } else if(IsKeyPressed(KEY_DOWN)) {
            state->selected_game = (state->selected_game + 1) % 2;
        }

        // Handle game selection with Enter/Space
        if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if(state->selected_game == 0) {
                RegisterPongGame(state);
                state->current_screen = GAMEBOX_PONG;
                state->prev_screen = GAMEBOX_MENU;
            } else if(state->selected_game == 1) {
                RegisterBreakoutGame(state);
                state->current_screen = GAMEBOX_BREAKOUT;
                state->prev_screen = GAMEBOX_MENU;
            }
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
                state->game_init = NULL;
                state->game_update = NULL;
                state->game_draw = NULL;
                state->game_close = NULL;
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

        // Game selection with highlighting
        Color pong_color = (state->selected_game == 0) ? YELLOW : WHITE;
        Color breakout_color = (state->selected_game == 1) ? YELLOW : WHITE;
        
        DrawText("1. PONG", GetScreenWidth()/2 - MeasureText("1. PONG", 50)/2, 300, 40, pong_color);
        DrawText("2. BREAKOUT", GetScreenWidth()/2 - MeasureText("2. BREAKOUT", 50)/2, 360, 40, breakout_color);
        DrawText("Use UP/DOWN arrows to select, ENTER to play", GetScreenWidth()/2 - MeasureText("Use UP/DOWN arrows to select, ENTER to play", 25)/2, 400, 25, GRAY);
        DrawText("Press ESC to exit", GetScreenWidth()/2 - MeasureText("Press ESC to exit", 20)/2, 430, 20, GRAY);
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
