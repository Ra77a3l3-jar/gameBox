#include <raylib.h>
#include <stdbool.h>
#include "breakout.h"

void BreakoutInit(BreakoutGameState *state) {
    BreakoutGameState *breakout_state = (BreakoutGameState*)state;

    breakout_state->current_screen = BREAKOUT_MENU;
    breakout_state->prev_screen = BREAKOUT_MENU;

    breakout_state->score = 0;
    breakout_state->level = 1;
    breakout_state->game_over = false;
    breakout_state->victory = false;

    breakout_state->paddle_width = PADDLE_WIDTH;
    breakout_state->paddle = (Rectangle) {GetScreenWidth()/2 - PADDLE_WIDTH/2, GetScreenHeight()/2 - 50, PADDLE_WIDTH, PADDLE_HEIGHT};
    breakout_state->paddle_speed = PADDLE_SPEED;

    breakout_state->ball_rad = BALL_RADIUS;
    breakout_state->ball_position = (Vector2){0, 0};
    breakout_state->ball_speed = (Vector2){0, 0};
    breakout_state->ball_active = false;

    breakout_state->brick_rows = BRICK_ROWS;
    breakout_state->brick_cols = BRICK_COLS;
    breakout_state->brick_width = (GetScreenWidth() - (BRICK_COLS + 1) * BRICK_PADD) / BRICK_COLS;
    breakout_state->brick_height = BRICK_HEIGHT;
    breakout_state->brick_count = 0;

    breakout_state->key_left = KEY_LEFT;
    breakout_state->key_right = KEY_RIGHT;

    BreakoutSetupLevel(breakout_state);
}

void BreakoutSetupLevel(BreakoutGameState *state) {
    state->brick_count = 0;
    float startX = BRICK_PADD;
    float startY = 50;

    for(int row = 0; row < state->brick_rows; row++) {
        for(int col = 0; col < state->brick_cols; col++) {
            int indx = row * state->brick_cols + col;
            state->bricks[indx] = (Rectangle) {
                startX + col * (state->brick_width + BRICK_PADD),
                startY + row * (state->brick_height + BRICK_PADD),
                state->brick_width,
                state->brick_height
            };
            state->bricks_active[indx] = true;
            state->brick_count++;
        }
    }
}

bool BreakoutUpdate(BreakoutGameState *state) {
    BreakoutGameState *breakout_state = (BreakoutGameState*)state;

    if(IsKeyPressed(KEY_ESCAPE)) {
        if(breakout_state->current_screen == BREAKOUT_GAMEPLAY) {
            breakout_state->prev_screen = breakout_state->current_screen;
            breakout_state->current_screen = BREAKOUT_PAUSED;
        } else if(breakout_state->current_screen == BREAKOUT_PAUSED) {
            breakout_state->current_screen = breakout_state->prev_screen;
        }

        if(breakout_state->current_screen == BREAKOUT_GAMEPLAY) {
            // Paddle movement
            if(IsKeyPressed(breakout_state->key_left) && breakout_state->paddle.x > 0) {
                breakout_state->paddle.x -= breakout_state->paddle_speed;
            }
            if(IsKeyPressed(breakout_state->key_right) && breakout_state->paddle.x + breakout_state->paddle_width < GetScreenWidth()) {
                breakout_state->paddle.x += breakout_state->paddle_speed;
            }

            // Ball movement
            if(breakout_state->ball_active) {
                breakout_state->ball_position.x += breakout_state->ball_speed.x;
                breakout_state->ball_position.y += breakout_state->ball_speed.y;

                // Ball collision wall

            }
        }
    }
}
