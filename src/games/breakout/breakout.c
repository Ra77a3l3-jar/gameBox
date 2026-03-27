#include <raylib.h>
#include <stdbool.h>
#include "breakout.h"

void BreakoutInit(BreakoutGameState *state) {
    BreakoutGameState *breakout_state = (BreakoutGameState*)state;

    breakout_state->current_screen = BREAKOUT_MENU;
    breakout_state->prev_screen = BREAKOUT_MENU;

    breakout_state->score = 0;
    breakout_state->level = 1;
    breakout_state->lives = 3;
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
            if(breakout_state->ball_position.x - breakout_state->ball_rad <= 0 ||
                breakout_state->ball_position.x + breakout_state->ball_rad >= GetScreenWidth()) {
                breakout_state->ball_speed.x *= -1;
            }

            if(breakout_state->ball_position.y - breakout_state->ball_rad <= 0) {
                breakout_state->ball_speed.y *= -1;
            }

            // Ball collision wiht bottom
            if(breakout_state->ball_position.y + breakout_state->ball_rad >= GetScreenHeight()) {
                breakout_state->lives--;
                breakout_state->ball_active = false;

                if(breakout_state->lives <= 0) {
                    breakout_state->game_over = true;
                    breakout_state->current_screen = BREAKOUT_MENU;
                }
            }

            // Ball collision with padle
            if(CheckCollisionCircleRec(breakout_state->ball_position, breakout_state->ball_rad, breakout_state->paddle)) {
                breakout_state->ball_speed.y *= -1;

                // Direction based on hit position
                float relative_intersection = (breakout_state->ball_position.x - breakout_state->paddle.x) / breakout_state->paddle_width;
                breakout_state->ball_speed.x = (relative_intersection - 0.5) * 10;
            }

            // Ball collison with bricks
            for(int i = 0; i < breakout_state->brick_count; i++) {
                if(breakout_state->bricks_active[i] &&
                    CheckCollisionCircleRec(breakout_state->ball_position, breakout_state->ball_rad, breakout_state->bricks[i])) {
                    breakout_state->bricks_active[i] = false;
                    breakout_state->score += 1;
                    breakout_state->ball_speed.y *= -1;

                    // Check all bricks destroyed
                    bool all_destroyed = false;
                    for(int j = 0; j < breakout_state->brick_count; j++) {
                        if(breakout_state->bricks_active[j]) {
                            all_destroyed = false;
                            break;
                        }
                    }

                    if(all_destroyed) {
                        breakout_state->victory = true;
                        breakout_state->current_screen = BREAKOUT_MENU;
                    }
                    break;
                }
            }
        }

        // Launch ball
        if(!breakout_state->ball_active && IsKeyPressed(KEY_SPACE)) {
            breakout_state->ball_active = true;
            breakout_state->ball_position = (Vector2) {
                breakout_state->paddle.x - breakout_state->paddle_width/2,
                breakout_state->paddle.y - breakout_state->ball_rad
            };
            breakout_state->ball_speed = (Vector2){0, -5};
        }
    }
    return true;
}

void BreakoutDraw(BreakoutGameState *state) {
    BreakoutGameState *breakout_state = (BreakoutGameState*)state;

    ClearBackground(BLACK);

    switch(breakout_state->current_screen) {
        case BREAKOUT_MENU: {
            DrawText("BREAKOUT", GetScreenWidth()/2 - MeasureText("BREAKOUT", 60)/2, GetScreenHeight()/4, 60, WHITE);
            DrawText("Press ENTER to Start", GetScreenWidth()/2 - MeasureText("Press ENTER to Start", 30)/2, GetScreenHeight()/2, 30, WHITE);

            DrawText("Press ESC to return", GetScreenWidth()/2 - MeasureText("Press ESC to return", 20)/2, GetScreenHeight()/2 + 50, 20, GRAY);
        }
        case BREAKOUT_GAMEPLAY: {
            // Draw brick
            for(int i = 0; i < breakout_state->brick_count; i++) {
                if(breakout_state->bricks_active[i]) {
                    Color color;

                    switch(i / breakout_state->brick_cols) {
                        case 0: color = RED; break;
                        case 1: color = ORANGE; break;
                        case 2: color = YELLOW; break;
                        case 3: color = GREEN; break;
                        default: color = BLUE;
                    }
                    DrawRectangleRec(breakout_state->bricks[i], color);
                }
            }

            // Draw paddle
            DrawRectangleRec(breakout_state->paddle, WHITE);

            // Draw ball
            DrawCircleV(breakout_state->ball_position, breakout_state->ball_rad, WHITE);

            // If ball not active instructions
            if(!breakout_state->ball_active) {
                DrawText("Press SPACE to Launch Ball", GetScreenWidth()/2 - MeasureText("Press SPACE to Launch Ball", 20)/2, GetScreenHeight()/2, 20, GRAY);
            }

            // Draw score and lives
            DrawText(TextFormat("Score: %d", breakout_state->score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %d", breakout_state->lives), GetScreenWidth() - 120, 10, 20, WHITE);
            DrawText(TextFormat("Level: %d", breakout_state->level), GetScreenWidth()/2 - 50, 10, 20, WHITE);
        }
        case BREAKOUT_PAUSED: {
            // Draw brick
            for(int i = 0; i < breakout_state->brick_count; i++) {
                if(breakout_state->bricks_active[i]) {
                    Color color;

                    switch(i / breakout_state->brick_cols) {
                        case 0: color = RED; break;
                        case 1: color = ORANGE; break;
                        case 2: color = YELLOW; break;
                        case 3: color = GREEN; break;
                        default: color = BLUE;
                    }
                    DrawRectangleRec(breakout_state->bricks[i], color);
                }
            }

            // Draw paddle
            DrawRectangleRec(breakout_state->paddle, WHITE);

            // Draw ball
            DrawCircleV(breakout_state->ball_position, breakout_state->ball_rad, WHITE);

            // Draw score and lives
            DrawText(TextFormat("Score: %d", breakout_state->score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %d", breakout_state->lives), GetScreenWidth() - 120, 10, 20, WHITE);
            DrawText(TextFormat("Level: %d", breakout_state->level), GetScreenWidth()/2 - 50, 10, 20, WHITE);

            DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 40)/2, GetScreenHeight()/2 - 20, 40, WHITE);
            DrawText("Press P to Resume", GetScreenWidth()/2 - MeasureText("Press P to Resume", 30)/2, GetScreenHeight()/2 + 40, 30, WHITE);
        }
    }
}
