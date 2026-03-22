#include <raylib.h>
#include <stdlib.h>
#include "gamebox.h"

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
