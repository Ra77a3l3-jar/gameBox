#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gamebox.h"
#include "pong.h"

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
        if(IsKeyPressed(KEY_ENTER)) {
            RegisterPongGame(state);
            state->current_screen = GAMEBOX_PONG;
            state->prev_screen = GAMEBOX_MENU;

            if(state->game_init) {
                state->game_init(state->game_state);
            }
        }
    } else {
        // Updating currenty running game
        if(state->game_update) {
            bool should_continue = state->game_update(state->game_state);
            if(!should_continue) {
                if(state->game_close) {
                    state->game_close(state->game_state);
                }
                free(state->game_state);
                state->game_state = NULL;
                state->current_screen = GAMEBOX_MENU;
                return;
            }
            
            state->game_update(state->game_state);
        }        

        if(IsKeyPressed(KEY_ESCAPE)) {
            if(state->game_close) {
                state->game_close(state->game_state);
            }
            free(state->game_state);
            state->current_screen = GAMEBOX_MENU;
            state->game_state = NULL;
        }
    }
}

void DrawGameBox(GameBoxState *state) {
    if(state->current_screen == GAMEBOX_MENU) {
        ClearBackground(BLACK);

        DrawText("GAMEBOX", GetScreenWidth()/2 - MeasureText("GAMEBOX", 80)/2, 100, 80, ORANGE);

        // Game selection
        DrawText("PONG", GetScreenWidth()/2 - MeasureText("PONG", 50)/2, 300, 50, WHITE);
        DrawText("Press ENTER to play", GetScreenWidth()/2 - MeasureText("Press ENTER to play", 25)/2, 400, 25, GRAY);
        DrawText("More games coming soon...", GetScreenWidth()/2 - MeasureText("More games coming soon...", 20)/2, 450, 20, GRAY);
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
