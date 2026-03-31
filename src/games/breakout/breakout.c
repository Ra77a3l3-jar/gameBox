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
    breakout_state->paddle = (Rectangle) {GetScreenWidth()/2 - PADDLE_WIDTH/2, GetScreenHeight() - 50, PADDLE_WIDTH, PADDLE_HEIGHT};
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
    breakout_state->selected_settings_section = BREAKOUT_SETTINGS_PADDLE_WIDTH;

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
            breakout_state->current_screen = BREAKOUT_GAMEPLAY;
        } else if(breakout_state->current_screen == BREAKOUT_SETTINGS) {
            breakout_state->current_screen = breakout_state->prev_screen;
        } else if(breakout_state->current_screen == BREAKOUT_SETTINGS) {
            return false;
        }
    }

    if(breakout_state->current_screen == BREAKOUT_MENU) {
        if(IsKeyPressed(KEY_ENTER)) {
            breakout_state->current_screen = BREAKOUT_GAMEPLAY;
            breakout_state->ball_active = false;
        } else if(IsKeyPressed(KEY_S)) {
            breakout_state->prev_screen = BREAKOUT_MENU;
            breakout_state->current_screen = BREAKOUT_SETTINGS;
        }
    }

    if(breakout_state->current_screen == BREAKOUT_SETTINGS) {
        if(IsKeyPressed(KEY_UP)) {
            breakout_state->selected_settings_section = (breakout_state->selected_settings_section - 1 + BREAKOUT_SETTINGS_SECTION_COUNT) % BREAKOUT_SETTINGS_SECTION_COUNT;
        } else if(IsKeyPressed(KEY_DOWN)) {
            breakout_state->selected_settings_section = (breakout_state->selected_settings_section + 1) % BREAKOUT_SETTINGS_SECTION_COUNT;
        }

        if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
            int direction = IsKeyPressed(KEY_LEFT) ? -1 : 1; // -1 left 1 right

            if(breakout_state->selected_settings_section == BREAKOUT_SETTINGS_PADDLE_WIDTH) {
                breakout_state->paddle_width += direction * 10;
                if(breakout_state->paddle_width < 30) breakout_state->paddle_width = 30;
                if(breakout_state->paddle_width > 200) breakout_state->paddle_width = 200;
            } else if(breakout_state->selected_settings_section == BREAKOUT_SETTINGS_ROWS) {
                breakout_state->brick_rows += direction;
                if(breakout_state->brick_rows < 2) breakout_state->brick_rows = 2;
                if(breakout_state->brick_rows > BRICK_MAX_ROWS) breakout_state->brick_rows = BRICK_MAX_ROWS;
                breakout_state->brick_width = (GetScreenWidth() - (breakout_state->brick_cols + 1) * BRICK_PADD) / breakout_state->brick_cols;
                BreakoutSetupLevel(breakout_state);
            } else if(breakout_state->selected_settings_section == BREAKOUT_SETTINGS_COLS) {
                breakout_state->brick_cols += direction;
                if(breakout_state->brick_cols < 5) breakout_state->brick_cols = 5;
                if(breakout_state->brick_cols > BRICK_MAX_COLS) breakout_state->brick_cols = BRICK_MAX_COLS;
                breakout_state->brick_width = (GetScreenWidth() - (breakout_state->brick_cols + 1) * BRICK_PADD) / breakout_state->brick_cols;
                BreakoutSetupLevel(breakout_state);
            } else if(breakout_state->selected_settings_section == BREAKOUT_SETTINGS_LIVES) {
                breakout_state->lives += direction;
                if(breakout_state->lives < 1) breakout_state->lives = 1;
                if(breakout_state->lives > LIVES_MAX) breakout_state->lives = LIVES_MAX;
            }
        }
    }

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
                case BREAKOUT_PAUSE_SETTINGS: {
                    breakout_state->prev_screen = BREAKOUT_PAUSED;
                    breakout_state->current_screen = BREAKOUT_SETTINGS;
                    break;
                }
                case BREAKOUT_PAUSE_QUIT: {
                    BreakoutInit(state);
                    break;
                }
                default: break;
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

            // Ball collision with walls
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

            // Ball collision with bricks
            for(int i = 0; i < breakout_state->brick_count; i++) {
                if(breakout_state->bricks_active[i] &&
                    CheckCollisionCircleRec(breakout_state->ball_position, breakout_state->ball_rad, breakout_state->bricks[i])) {
                    breakout_state->bricks_active[i] = false;
                    breakout_state->score += 1;
                    breakout_state->ball_speed.y *= -1;

                    // Check if all are destroyed
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
        if(!breakout_state->ball_active && IsKeyPressed(KEY_SPACE)) {
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
            DrawText("Press ENTER to Start", GetScreenWidth()/2 - MeasureText("Press ENTER to Start", 30)/2, GetScreenHeight()/2, 30, WHITE);
            DrawText("Press S for Settings", GetScreenWidth()/2 - MeasureText("Press S for Settings", 30)/2, GetScreenHeight()/2 + 50, 30, WHITE);
            DrawText("Press ESC to return", GetScreenWidth()/2 - MeasureText("Press ESC to return", 20)/2, GetScreenHeight()/2 + 100, 20, GRAY);
            break;
        }
        case BREAKOUT_SETTINGS: {
            DrawText("SETTINGS", GetScreenWidth()/2 - MeasureText("SETTINGS", 40)/2, 50, 40, WHITE);

            int start_y = 150;
            int section_spacing = 100;

            int pw_y = start_y;
            Color pw_color = (breakout_state->selected_settings_section == BREAKOUT_SETTINGS_PADDLE_WIDTH) ? YELLOW : WHITE;
            DrawText("Paddle Width", GetScreenWidth()/2 - MeasureText("Paddle Width", 30)/2, pw_y, 30, pw_color);
            DrawText(TextFormat("%d", breakout_state->paddle_width), GetScreenWidth()/2 - MeasureText("100", 30)/2, pw_y + 40, 30, GREEN);

            int rows_y = start_y + section_spacing;
            Color rows_color = (breakout_state->selected_settings_section == BREAKOUT_SETTINGS_ROWS) ? YELLOW : WHITE;
            DrawText("Rows", GetScreenWidth()/2 - MeasureText("Rows", 30)/2, rows_y, 30, rows_color);
            DrawText(TextFormat("%d", breakout_state->brick_rows), GetScreenWidth()/2 - MeasureText("8", 30)/2, rows_y + 40, 30, GREEN);

            int cols_y = start_y + section_spacing * 2;
            Color cols_color = (breakout_state->selected_settings_section == BREAKOUT_SETTINGS_COLS) ? YELLOW : WHITE;
            DrawText("Columns", GetScreenWidth()/2 - MeasureText("Columns", 30)/2, cols_y, 30, cols_color);
            DrawText(TextFormat("%d", breakout_state->brick_cols), GetScreenWidth()/2 - MeasureText("15", 30)/2, cols_y + 40, 30, GREEN);

            int lives_y = start_y + section_spacing * 3;
            Color lives_color = (breakout_state->selected_settings_section == BREAKOUT_SETTINGS_LIVES) ? YELLOW : WHITE;
            DrawText("Lives", GetScreenWidth()/2 - MeasureText("Lives", 30)/2, lives_y, 30, lives_color);
            DrawText(TextFormat("%d", breakout_state->lives), GetScreenWidth()/2 - MeasureText("9", 30)/2, lives_y + 40, 30, GREEN);

            DrawText("Use UP/DOWN to navigate, LEFT/RIGHT to adjust", GetScreenWidth()/2 - MeasureText("Use UP/DOWN to navigate, LEFT/RIGHT to adjust", 20)/2, GetScreenHeight() - 50, 20, GRAY);
            DrawText("Press ESC to return", GetScreenWidth()/2 - MeasureText("Press ESC to return", 20)/2, GetScreenHeight() - 20, 20, WHITE);
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
                        case 4: color = BLUE; break;
                        case 5: color = PURPLE; break;
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
                DrawText("Press SPACE to Launch Ball", GetScreenWidth()/2 - MeasureText("Press SPACE to Launch Ball", 20)/2, GetScreenHeight()/2, 20, GRAY);
            }

            DrawText(TextFormat("Score: %d", breakout_state->score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %d", breakout_state->lives), GetScreenWidth() - 150, 10, 20, WHITE);
            DrawText(TextFormat("Level: %d", breakout_state->level), GetScreenWidth()/2 - 50, 10, 20, WHITE);
            break;
        }
        case BREAKOUT_PAUSED: {
            for(int i = 0; i < breakout_state->brick_count; i++) {
                if(breakout_state->bricks_active[i]) {
                    Color color;

                    switch(i / breakout_state->brick_cols) {
                        case 0: color = RED; break;
                        case 1: color = ORANGE; break;
                        case 2: color = YELLOW; break;
                        case 3: color = GREEN; break;
                        case 4: color = BLUE; break;
                        case 5: color = PURPLE; break;
                    }
                    DrawRectangleRec(breakout_state->bricks[i], color);
                }
            }

            DrawRectangleRec(breakout_state->paddle, WHITE);
            DrawCircleV(breakout_state->ball_position, breakout_state->ball_rad, WHITE);

            DrawText(TextFormat("Score: %d", breakout_state->score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %d", breakout_state->lives), GetScreenWidth() - 150, 10, 20, WHITE);
            DrawText(TextFormat("Level: %d", breakout_state->level), GetScreenWidth()/2 - 50, 10, 20, WHITE);

            DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 40)/2, GetScreenHeight()/2 - 100, 40, WHITE);

            int menu_start_y = GetScreenHeight()/2 - 20;
            int menu_spacing = 50;

            Color resume_color = (breakout_state->selected_pause == BREAKOUT_PAUSE_RESUME) ? YELLOW : WHITE;
            Color restart_color = (breakout_state->selected_pause == BREAKOUT_PAUSE_RESTART) ? YELLOW : WHITE;
            Color settings_color = (breakout_state->selected_pause == BREAKOUT_PAUSE_SETTINGS) ? YELLOW : WHITE;
            Color quit_color = (breakout_state->selected_pause == BREAKOUT_PAUSE_QUIT) ? YELLOW : WHITE;

            DrawText("Resume", GetScreenWidth()/2 - MeasureText("Resume", 30)/2, menu_start_y, 30, resume_color);
            DrawText("Restart", GetScreenWidth()/2 - MeasureText("Restart", 30)/2, menu_start_y + menu_spacing, 30, restart_color);
            DrawText("Settings", GetScreenWidth()/2 - MeasureText("Settings", 30)/2, menu_start_y + menu_spacing * 2, 30, settings_color);
            DrawText("Quit to Menu", GetScreenWidth()/2 - MeasureText("Quit to Menu", 30)/2, menu_start_y + menu_spacing * 3, 30, quit_color);

            DrawText("Use UP/DOWN to navigate, ENTER to select", GetScreenWidth()/2 - MeasureText("Use UP/DOWN to navigate, ENTER to select", 20)/2, GetScreenHeight() - 50, 20, GRAY);
            break;
        }
    }
}

void BreakoutClose(BreakoutGameState *state) {

}
