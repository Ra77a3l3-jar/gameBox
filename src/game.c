#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include "game.h"

void InitGame(GameState *state) {
    state->current_screen = MENU;
    state->vs_computer = false;

    state->player_paddle = (Rectangle){20, GetScreenHeight()/2 - 50, 20, 100};
    state->oponent_paddle = (Rectangle){GetScreenWidth() - 40, GetScreenHeight()/2 - 50, 20, 100};

    state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
    state->ball_speed = (Vector2){5, 3};
    
    state->player_score = 0;
    state->oponent_score = 0;
    state->game_over = false;

    state->paddle_hit_sound = LoadSound("assets/audio/pong_paddle_hit.wav");
    state->score_sound = LoadSound("assets/audio/pong_score.wav");
    state->wall_hit_sound = LoadSound("assets/audio/pong_wall_hit.wav");
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
        }
    }
    
    if(state->current_screen == GAMEPLAY) {
        // Player movement
        if(IsKeyDown(KEY_W) && state->player_paddle.y > 0) {
            state->player_paddle.y -= 7;
        }
        if(IsKeyDown(KEY_D) && state->player_paddle.y + state->player_paddle.height < GetScreenHeight()) {
            state->player_paddle.y += 7;
        }

        // Computer movement
        if(state->vs_computer) {
            float computer_movement_speed = 2.5; // Maximum pixels per frame (increase for higher difficulty)

            if(state->ball_speed.x > 0) { // React only if ball going towords oponent paddle
                float error = (rand() % 21) - 10; // Random ofset to simulate imperfection (-10 10)
                float target = state->ball_position.y - state->oponent_paddle.height/2 + error;
                // Position where the paddle wants to get (aligns paddle with ball centre plus the offset)

                // Paddle movement towwords target
                if(state->oponent_paddle.y < target) {
                    state->oponent_paddle.y += computer_movement_speed;
                } else if(state->oponent_paddle.y > target) {
                    state->oponent_paddle.y -= computer_movement_speed;
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

        // Ball movement
        state->ball_position.x += state->ball_speed.x;
        state->ball_position.y += state->ball_speed.y;

        // Ball collision with Top and Botom
        if (state->ball_position.y <= 0 || state->ball_position.y >= GetScreenHeight()) {
            state->ball_speed.y *= -1;
            PlaySound(state->wall_hit_sound);
        }

        // Ball collision with Paddle
        if (CheckCollisionCircleRec(state->ball_position, 10, state->player_paddle) ||
            CheckCollisionCircleRec(state->ball_position, 10, state->oponent_paddle)) {
            state->ball_speed.x *= -1;
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
    }
}

void DrawGame(GameState *state) {
    switch(state->current_screen) {
        case MENU: {
                DrawText("PONG", GetScreenWidth()/2 - MeasureText("PONG", 60)/2, GetScreenHeight()/4, 60, WHITE);

                // Draw options
                DrawText("Press 1 for PvP", GetScreenWidth()/2 - MeasureText("Press 1 for PvP", 30)/2, GetScreenHeight()/2, 30, WHITE);
                DrawText("Press 2 for PvE", GetScreenWidth()/2 - MeasureText("Press 2 for PvE", 30)/2, GetScreenHeight()/2 + 40, 30, WHITE);
                if (IsKeyPressed(KEY_ONE)) {
                    state->vs_computer = false;
                    state->current_screen = GAMEPLAY;
                } else if (IsKeyPressed(KEY_TWO)) {
                    state->vs_computer = true;
                    state->current_screen = GAMEPLAY;
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
                DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 40)/2, GetScreenHeight()/2, 40, WHITE);

                // Draw options
                DrawText("Press ESC to resume", GetScreenWidth()/2 - MeasureText("Press ESC to resume", 20)/2, GetScreenHeight()/2 + 40, 20, WHITE);
                DrawText("Press M to return to menu", GetScreenWidth()/2 - MeasureText("Press M to return to menu", 20)/2, GetScreenHeight()/2 + 70, 20, WHITE);
                if (IsKeyPressed(KEY_M)) {
                    state->current_screen = MENU;
                    InitGame(state); // Reset game state
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
