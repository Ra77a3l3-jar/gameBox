#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include "game.h"

#define DIFFICULTY_LEVELS 6
float difficulty_speeds[DIFFICULTY_LEVELS] = {2.3, 2.6, 2.9, 3.2, 3.5, 4.5};

#define VICTORY_POINTS_OPTIONS 4
int victory_points_options[VICTORY_POINTS_OPTIONS] = {5, 10, 15, 20};

void InitGame(GameState *state) {
    state->current_screen = MENU;
    state->prev_screen = MENU;
    
    state->vs_computer = false;

    state->player_paddle = (Rectangle){20, GetScreenHeight()/2 - 50, 20, 100};
    state->oponent_paddle = (Rectangle){GetScreenWidth() - 40, GetScreenHeight()/2 - 50, 20, 100};

    state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
    state->ball_speed = (Vector2){5, 3};
    
    state->player_score = 0;
    state->oponent_score = 0;
    state->game_over = false;

    state->difficulty_level = 2;
    state->victory_points = 10;
    state->computer_movement_speed = difficulty_speeds[state->difficulty_level];

    state->paddle_hit_sound = LoadSound("assets/audio/pong_paddle_hit.wav");
    state->score_sound = LoadSound("assets/audio/pong_score.wav");
    state->wall_hit_sound = LoadSound("assets/audio/pong_wall_hit.wav");

    state->selected_pause = PAUSE_RESUME;
}

void UpdateGame(GameState *state) {

    if(IsWindowResized()) {
        // Keep player paddle on left side (x stays at 20)
        // Update opponent paddle to stay on right side
        state->oponent_paddle.x = GetScreenWidth() - 40;
        
        // Adjust paddle Y position to suite new screen bounds
        if(state->player_paddle.y + state->player_paddle.height > GetScreenHeight()) {
            state->player_paddle.y = GetScreenHeight() - state->player_paddle.height;
        }
        if(state->oponent_paddle.y + state->oponent_paddle.height > GetScreenHeight()) {
            state->oponent_paddle.y = GetScreenHeight() - state->oponent_paddle.height;
        }
        
        // Adjust ball position to new screen bounds
        if(state->ball_position.x > GetScreenWidth()) {
            state->ball_position.x = GetScreenWidth() - 20;
        }
        if(state->ball_position.y > GetScreenHeight()) {
            state->ball_position.y = GetScreenHeight() - 20;
        }
    }

    if(IsKeyPressed(KEY_ESCAPE)) {
        if(state->current_screen == GAMEPLAY) {
            state->current_screen = PAUSED;
        } else if(state->current_screen == PAUSED) {
            state->current_screen = GAMEPLAY;
        } else if(state->current_screen == SETTINGS) {
            state->current_screen = state->prev_screen;
        } else if(state->current_screen == MENU) {
            exit(0);
        }
    }

    if(state->current_screen == PAUSED) {
        if(IsKeyPressed(KEY_UP)) {
            state->selected_pause =(state->selected_pause + PAUSE_OPTION_COUT - 1) % PAUSE_OPTION_COUT;
        } else if(IsKeyPressed(KEY_DOWN)) {
            state->selected_pause = (state->selected_pause + 1) % PAUSE_OPTION_COUT;
        } else if(IsKeyPressed(KEY_ENTER)) {
            switch(state->selected_pause) {
                case PAUSE_RESUME: {
                        state->current_screen = GAMEPLAY;
                        break;
                }
                case PAUSE_RESTART: {
                        bool vs_computer = state->vs_computer;
                        InitGame(state);
                        state->vs_computer = vs_computer;
                        state->current_screen = GAMEPLAY;
                        break;      
                }
                case PAUSE_SETTINGS: {
                        state->prev_screen = PAUSED;
                        state->current_screen = SETTINGS;
                        break;
                }
                case PAUSE_QUIT: {
                        state->current_screen = MENU;
                        InitGame(state);
                        break;        
                }
            }
        }
    }
    
    if(state->current_screen == GAMEPLAY) {
        // Player movement
        float prev_player_y = state->player_paddle.y;
        float prev_oponent_y = state->oponent_paddle.y;
        
        if(IsKeyDown(KEY_W) && state->player_paddle.y > 0) {
            state->player_paddle.y -= 7;
        }
        if(IsKeyDown(KEY_D) && state->player_paddle.y + state->player_paddle.height < GetScreenHeight()) {
            state->player_paddle.y += 7;
        }
        state->player_paddle_speed = state->player_paddle.y - prev_player_y;

        // Computer movement
        if(state->vs_computer) {
            if(state->ball_speed.x > 0) { // React only if ball going towords oponent paddle
                float error = (float)GetRandomValue(-10, 10);
                float target = state->ball_position.y - state->oponent_paddle.height/2 + error;
                // Position where the paddle wants to get (aligns paddle with ball centre plus the offset)

                // Paddle movement towwords target
                if(state->oponent_paddle.y < target) {
                    state->oponent_paddle.y += state->computer_movement_speed;
                } else if(state->oponent_paddle.y > target) {
                    state->oponent_paddle.y -= state->computer_movement_speed;
                }
            }
        } else {
            if (IsKeyDown(KEY_UP) && state->oponent_paddle.y > 0) {
                state->oponent_paddle.y -= 7;
            }
            if (IsKeyDown(KEY_DOWN) && state->oponent_paddle.y + state->oponent_paddle.height < GetScreenHeight()) {
                state->oponent_paddle.y += 7;
            }
        }
        state->oponent_paddle_speed = state->oponent_paddle.y - prev_oponent_y;

        // Ball movement
        state->ball_position.x += state->ball_speed.x;
        state->ball_position.y += state->ball_speed.y;

        // Ball collision with Top and Botom
        if (state->ball_position.y <= 0 || state->ball_position.y >= GetScreenHeight()) {
            state->ball_speed.y *= -1;

            // Randomize a bit the bouncing from colliding with the wall
            float randomFactor = ((float)GetRandomValue(-30, 30) / 100.0f);
            state->ball_position.x += randomFactor;
            
            PlaySound(state->wall_hit_sound);
        }

        // Ball collision with Paddle
        if (CheckCollisionCircleRec(state->ball_position, 10, state->player_paddle) ||
            CheckCollisionCircleRec(state->ball_position, 10, state->oponent_paddle)) {

            // Calculate relative intersection point (0.0 1.0)
            Rectangle *paddle = CheckCollisionCircleRec(state->ball_position, 10, state->player_paddle) ? &state->player_paddle : &state->oponent_paddle;
            float relative_y_intersection = (state->ball_position.y - paddle->y) / paddle->height;

            float paddle_speed = (paddle == &state->player_paddle) ? state->player_paddle_speed : state->oponent_paddle_speed;

            /* Adjust ball speed based on where it hit
               The middle of the paddle does not change the y of balls trajectory
               Top and bottom of paddle increase y speed */

            state->ball_speed.y = (relative_y_intersection - 0.5) * 10 + paddle_speed * 0.2; // Scale factor of the bounce angle

            state->ball_speed.x *= -1.0; // Just reverse movement

            if(fabsf(paddle_speed) > 0.1) { // If paddle is moving
                state->ball_speed.x *= 1.1; // Increse by 10% speed
            } else { // If paddle is static
                state->ball_speed.x *= 0.90; // Decrease by 10% speed
            }

            PlaySound(state->paddle_hit_sound);
        }

        // Scoring
        if (state->ball_position.x <= 0) {
            state->oponent_score++;
            state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
            state->ball_speed = (Vector2){5, 3};
            PlaySound(state->score_sound);
        } else if (state->ball_position.x >= GetScreenWidth()) {
            state->player_score++;
            state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
            state->ball_speed = (Vector2){-5, 3};
            PlaySound(state->score_sound);
        }

        // Check for Victory
        if(state->player_score >= state->victory_points) {
            state->victory_player = 0;
            state->victory_timer = 240; // 2s at 120fps
            state->current_screen = VICTORY;
        } else if(state->oponent_score >= state->victory_points) {
            state->victory_player = 1;
            state->victory_timer = 240;
            state->current_screen = VICTORY;
        }
    }
}

void DrawGame(GameState *state) {
    switch(state->current_screen) {
        case MENU: {
                DrawText("PONG", GetScreenWidth()/2 - MeasureText("PONG", 60)/2, GetScreenHeight()/4, 60, WHITE);

                // Draw options
                DrawText("Press 1 for PvP", GetScreenWidth()/2 - MeasureText("Press 1 for PvP", 30)/2, GetScreenHeight()/2, 30, WHITE);
                DrawText("Press 2 for PvE", GetScreenWidth()/2 - MeasureText("Press 2 for PvE", 30)/2, GetScreenHeight()/2 + 40, 30, WHITE);
                DrawText("Press S for Settings", GetScreenWidth()/2 - MeasureText("Press S for Settings", 30)/2, GetScreenHeight()/2 + 80, 30, WHITE);
                DrawText("Press ESC to Exit", GetScreenWidth()/2 - MeasureText("Press ESC to Exit", 30)/2, GetScreenHeight()/2 + 120, 30, WHITE);
                if (IsKeyPressed(KEY_ONE)) {
                    state->vs_computer = false;
                    state->current_screen = GAMEPLAY;
                } else if (IsKeyPressed(KEY_TWO)) {
                    state->vs_computer = true;
                    state->current_screen = GAMEPLAY;
                } else if (IsKeyPressed(KEY_S)) {
                    state->current_screen = SETTINGS;
                }
                break;
        }
        case GAMEPLAY: {
                // Player paddle texture
                DrawRectangleRec(state->player_paddle, WHITE);
                DrawRectangleRec(state->oponent_paddle, WHITE);

                // Draw ball
                DrawCircleV(state->ball_position, 10, WHITE);
                
                // Draw points
                DrawText(TextFormat("%d", state->player_score), GetScreenWidth()/4, 50, 30, WHITE);
                DrawText(TextFormat("%d", state->oponent_score), 3*GetScreenWidth()/4, 50, 30, WHITE);
                DrawText(TextFormat("%d", GetFPS()), 2*GetScreenWidth()/4, 50, 30, WHITE);
                break;
        }
        case PAUSED: {
                DrawRectangleRec(state->player_paddle, WHITE);
                DrawRectangleRec(state->oponent_paddle, WHITE);
                DrawCircleV(state->ball_position, 10, WHITE);
                DrawText(TextFormat("%d", state->player_score), GetScreenWidth()/4, 50, 30, WHITE);
                DrawText(TextFormat("%d", state->oponent_score), 3*GetScreenWidth()/4, 50, 30, WHITE);

                DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 40)/2, GetScreenHeight()/2 - 100, 40, WHITE);

                // Draw pause menu options
                char *pause_options[PAUSE_OPTION_COUT] = {"Resume", "Restart", "Settings", "Quit to Menu"};
                for (int i = 0; i < PAUSE_OPTION_COUT; i++) {
                    Color color = (i == state->selected_pause) ? RED : WHITE;
                    DrawText(pause_options[i], GetScreenWidth()/2 - MeasureText(pause_options[i], 30)/2, GetScreenHeight()/2 - 20 + i * 40, 30, color);
                }
                break;
        }
        case SETTINGS: {
                DrawText("SETTINGS", GetScreenWidth()/2 - MeasureText("SETTINGS", 40)/2, 100, 40, WHITE);

                // Difficulty slider
                DrawText("Difficulty", 200, 200, 25, WHITE);
                for(int i = 0; i < DIFFICULTY_LEVELS; i++) {
                    Color color = (i == state->difficulty_level) ? GREEN : GRAY;
                    DrawRectangle(400 + i * 50, 200, 20, 20, color);
                    DrawText(TextFormat("%d", i + 1), 400 + i * 50, 230, 15, WHITE);
                }
                if(IsKeyPressed(KEY_LEFT) && state->difficulty_level > 0) {
                    state->difficulty_level--;
                    state->computer_movement_speed = difficulty_speeds[state->difficulty_level];
                }
                if(IsKeyPressed(KEY_RIGHT) && state->difficulty_level < DIFFICULTY_LEVELS - 1) {
                    state->difficulty_level++;
                    state->computer_movement_speed = difficulty_speeds[state->difficulty_level];
                }

                // Victory point slider
                DrawText("Victory Points", 200, 300, 25, WHITE);
                for(int i = 0; i < VICTORY_POINTS_OPTIONS; i++) {
                    Color color = (victory_points_options[i] == state->victory_points) ? RED : GRAY;
                    DrawRectangle(400 + i * 50, 300, 20, 20, color);
                    DrawText(TextFormat("%d", victory_points_options[i]), 400 + i * 50, 330, 15, WHITE);
                }
                if(IsKeyPressed(KEY_UP)) {
                    for(int i = 0; i < VICTORY_POINTS_OPTIONS; i++) {
                        if(victory_points_options[i] == state->victory_points) {
                            state->victory_points = victory_points_options[(i + VICTORY_POINTS_OPTIONS - 1) % VICTORY_POINTS_OPTIONS];
                            break;
                        }
                    }
                }
                if(IsKeyPressed(KEY_DOWN)) {
                    for(int i = 0; i < VICTORY_POINTS_OPTIONS; i++) {
                        if(victory_points_options[i] == state->victory_points) {
                        state->victory_points = victory_points_options[(i + 1) % VICTORY_POINTS_OPTIONS];
                        break;
                        }
                    }
                }

                DrawText("Press ESC to return to menu", GetScreenWidth()/2 - MeasureText("Press ESC to return to menu", 20)/2, 500, 20, WHITE);
                break;                                
        }
        case VICTORY: {
                const char *winner = (state->victory_player == 0) ? "PLAYER 1 WINS!" : "PLAYER 2 WINS!";

                DrawText(winner, GetScreenWidth()/2 - MeasureText(winner, 50)/2, GetScreenHeight()/2 - 50, 50, GREEN);

                DrawText("Press M to return to menu", GetScreenWidth()/2 - MeasureText("Press M to return to menu", 30)/2, GetScreenHeight()/2 + 50, 30, WHITE);

                state->victory_timer--;
                if(state->victory_timer <= 0 || IsKeyPressed(KEY_M)) {
                    state->current_screen = MENU;
                    InitGame(state);
                }
                break;        
        }
    }
}

void CloseGame(GameState *state) {
    UnloadSound(state->paddle_hit_sound);
    UnloadSound(state->score_sound);
    UnloadSound(state->wall_hit_sound);
}
