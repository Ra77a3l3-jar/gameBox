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

    breakout_state->brick_rows = BRICK_ROWS;
    breakout_state->brick_cols = BRICK_COLS;
    breakout_state->brick_width = (GetScreenWidth() - (BRICK_COLS + 1) * BRICK_PADD) / BRICK_COLS;
    breakout_state->brick_height = BRICK_HEIGHT;
    breakout_state->brick_count = 0;

    breakout_state->key_left = KEY_LEFT;
    breakout_state->key_right = KEY_RIGHT;
}
