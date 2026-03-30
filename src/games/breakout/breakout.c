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
    breakout_state->paddle = (Rectangle) {
        GetScreenWidth()/2 - PADDLE_WIDTH/2,
        GetScreenHeight() - 50,
        PADDLE_WIDTH,
        PADDLE_HEIGHT
    };
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

    breakout_state->selected_pause = BREAKOUT_PAUSE_RESUME;

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

    // ESC key handling
    if(IsKeyPressed(KEY_ESCAPE)) {
        if(breakout_state->current_screen == BREAKOUT_GAMEPLAY) {
            breakout_state->prev_screen = breakout_state->current_screen;
            breakout_state->current_screen = BREAKOUT_PAUSED;
        } else if(breakout_state->current_screen == BREAKOUT_PAUSED) {
            breakout_state->current_screen = BREAKOUT_GAMEPLAY;
        } else if(breakout_state->current_screen == BREAKOUT_MENU) {
            // Return to main menu
            return false;
        }
    }

    if(breakout_state->current_screen == BREAKOUT_MENU) {
        if(IsKeyPressed(KEY_SPACE)) {
            breakout_state->current_screen = BREAKOUT_GAMEPLAY;
        }
    }

    // Pause menu navigation
    if(breakout_state->current_screen == BREAKOUT_PAUSED) {
        if(IsKeyPressed(KEY_UP)) {
            breakout_state->selected_pause = (breakout_state->selected_pause + BREAKOUT_PAUSE_OPTION_COUNT - 1) % BREAKOUT_PAUSE_OPTION_COUNT;
        } else if(IsKeyPressed(KEY_DOWN)) {
            breakout_state->selected_pause = (breakout_state->selected_pause + 1) % BREAKOUT_PAUSE_OPTION_COUNT;
        } else if(IsKeyPressed(KEY_ENTER)) {
            switch(breakout_state->selected_pause) {
                case BREAKOUT_PAUSE_RESUME: {
                    breakout_state->current_screen = BREAKOUT_GAMEPLAY;
                    break;
                }
                case BREAKOUT_PAUSE_RESTART: {
                    BreakoutInit(state);
                    breakout_state->current_screen = BREAKOUT_GAMEPLAY;
                    break;
                }
                case BREAKOUT_PAUSE_QUIT: {
                    breakout_state->current_screen = BREAKOUT_MENU;
                    BreakoutInit(state);
                    break;
                }
            }
        }
    }

    if(breakout_state->current_screen == BREAKOUT_GAMEPLAY) {
        // Paddle movement
        if(IsKeyDown(breakout_state->key_left) && breakout_state->paddle.x > 0) {
            breakout_state->paddle.x -= breakout_state->paddle_speed;
        }
        if(IsKeyDown(breakout_state->key_right) && breakout_state->paddle.x + breakout_state->paddle_width < GetScreenWidth()) {
            breakout_state->paddle.x += breakout_state->paddle_speed;
        }

        // Ball movement
        if(breakout_state->ball_active) {
            breakout_state->ball_position.x += breakout_state->ball_speed.x;
            breakout_state->ball_position.y += breakout_state->ball_speed.y;

            // Ball collision with wall
            if(breakout_state->ball_position.x - breakout_state->ball_rad <= 0 ||
                breakout_state->ball_position.x + breakout_state->ball_rad >= GetScreenWidth()) {
                breakout_state->ball_speed.x *= -1;
            }

            if(breakout_state->ball_position.y - breakout_state->ball_rad <= 0) {
                breakout_state->ball_speed.y *= -1;
            }

            // Ball collision with bottom
            if(breakout_state->ball_position.y + breakout_state->ball_rad >= GetScreenHeight()) {
                breakout_state->lives--;
                breakout_state->ball_active = false;

                if(breakout_state->lives <= 0) {
                    breakout_state->game_over = true;
                    breakout_state->current_screen = BREAKOUT_MENU;
                }
            }

            // Ball collision with paddle
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

                    // Check if all bricks destroyed
                    bool all_destroyed = true;
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
        if(!breakout_state->ball_active && IsKeyPressed(KEY_ENTER)) {
            breakout_state->ball_active = true;
            breakout_state->ball_position = (Vector2) {
                breakout_state->paddle.x + breakout_state->paddle_width/2,
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
            DrawText("Press SPACE to Start", GetScreenWidth()/2 - MeasureText("Press SPACE to Start", 30)/2, GetScreenHeight()/2, 30, WHITE);
            DrawText("Press ESC to return", GetScreenWidth()/2 - MeasureText("Press ESC to return", 20)/2, GetScreenHeight()/2 + 50, 20, GRAY);
            break;
        }
        case BREAKOUT_GAMEPLAY: {
            for(int i = 0; i < breakout_state->brick_count; i++) {
                if(breakout_state->bricks_active[i]) {
                    Color color;

                    switch(i / breakout_state->brick_cols) {
                        case 0: color = RED; break;
                        case 1: color = ORANGE; break;
                        case 2: color = YELLOW; break;
                        case 3: color = GREEN; break;
                        default: color = BLUE; break;
                    }
                    DrawRectangleRec(breakout_state->bricks[i], color);
                }
            }

            // Draw paddle
            DrawRectangleRec(breakout_state->paddle, WHITE);

            // Draw ball
            DrawCircleV(breakout_state->ball_position, breakout_state->ball_rad, WHITE);

            // Instructions
            if(!breakout_state->ball_active) {
                DrawText("Press ENTER to Launch Ball", GetScreenWidth()/2 - MeasureText("Press ENTER to Launch Ball", 20)/2, GetScreenHeight()/2, 20, GRAY);
            }

            // Draw score, lives, and level
            DrawText(TextFormat("Score: %d", breakout_state->score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %d", breakout_state->lives), GetScreenWidth() - 150, 10, 20, WHITE);
            DrawText(TextFormat("Level: %d", breakout_state->level), GetScreenWidth()/2 - 50, 10, 20, WHITE);
            break;
        }
        case BREAKOUT_PAUSED: {
            // Draw bricks
            for(int i = 0; i < breakout_state->brick_count; i++) {
                if(breakout_state->bricks_active[i]) {
                    Color color;

                    switch(i / breakout_state->brick_cols) {
                        case 0: color = RED; break;
                        case 1: color = ORANGE; break;
                        case 2: color = YELLOW; break;
                        case 3: color = GREEN; break;
                        default: color = BLUE; break;
                    }
                    DrawRectangleRec(breakout_state->bricks[i], color);
                }
            }

            // Draw paddle
            DrawRectangleRec(breakout_state->paddle, WHITE);

            // Draw ball
            DrawCircleV(breakout_state->ball_position, breakout_state->ball_rad, WHITE);

            // Draw score, lives, and level
            DrawText(TextFormat("Score: %d", breakout_state->score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %d", breakout_state->lives), GetScreenWidth() - 150, 10, 20, WHITE);
            DrawText(TextFormat("Level: %d", breakout_state->level), GetScreenWidth()/2 - 50, 10, 20, WHITE);

            DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 40)/2, GetScreenHeight()/2 - 100, 40, WHITE);

            int menu_start_y = GetScreenHeight()/2 - 20;
            int menu_spacing = 50;

            Color resume_color = (breakout_state->selected_pause == BREAKOUT_PAUSE_RESUME) ? YELLOW : WHITE;
            Color restart_color = (breakout_state->selected_pause == BREAKOUT_PAUSE_RESTART) ? YELLOW : WHITE;
            Color quit_color = (breakout_state->selected_pause == BREAKOUT_PAUSE_QUIT) ? YELLOW : WHITE;

            DrawText("Resume", GetScreenWidth()/2 - MeasureText("Resume", 30)/2, menu_start_y, 30, resume_color);
            DrawText("Restart", GetScreenWidth()/2 - MeasureText("Restart", 30)/2, menu_start_y + menu_spacing, 30, restart_color);
            DrawText("Quit to Menu", GetScreenWidth()/2 - MeasureText("Quit to Menu", 30)/2, menu_start_y + menu_spacing * 2, 30, quit_color);

            DrawText("Use UP/DOWN to navigate, ENTER to select", GetScreenWidth()/2 - MeasureText("Use UP/DOWN to navigate, ENTER to select", 20)/2, GetScreenHeight() - 50, 20, GRAY);
            break;
        }
    }
}

void BreakoutClose(BreakoutGameState *state) {

}
