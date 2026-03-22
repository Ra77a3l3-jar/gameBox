#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "pong.h"
#include "utils.h"

#define DIFFICULTY_LEVELS 6
float difficulty_speeds[DIFFICULTY_LEVELS] = {2.3, 2.6, 2.9, 3.2, 3.5, 4.5};

#define VICTORY_POINTS_OPTIONS 4
int victory_points_options[VICTORY_POINTS_OPTIONS] = {5, 10, 15, 20};

void PongInit(PongGameState *state) {
    PongGameState *pong_state = (PongGameState*)state;
    
    pong_state->current_screen = MENU;
    pong_state->prev_screen = MENU;
    
    pong_state->vs_computer = false;

    pong_state->player_paddle = (Rectangle){20, GetScreenHeight()/2 - 50, 20, 100};
    pong_state->oponent_paddle = (Rectangle){GetScreenWidth() - 40, GetScreenHeight()/2 - 50, 20, 100};

    pong_state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
    pong_state->ball_speed = (Vector2){5, 3};
    
    pong_state->player_score = 0;
    pong_state->oponent_score = 0;
    pong_state->game_over = false;

    pong_state->difficulty_level = 2;
    pong_state->victory_points = 10;
    pong_state->computer_movement_speed = difficulty_speeds[pong_state->difficulty_level];

    pong_state->paddle_hit_sound = LoadSound("assets/audio/pong_paddle_hit.wav");
    pong_state->score_sound = LoadSound("assets/audio/pong_score.wav");
    pong_state->wall_hit_sound = LoadSound("assets/audio/pong_wall_hit.wav");

    pong_state->selected_pause = PAUSE_RESUME;

    pong_state->key_player_up = KEY_W;
    pong_state->key_player_down = KEY_S;
    pong_state->key_opponent_up = KEY_UP;
    pong_state->key_opponent_down = KEY_DOWN;
    pong_state->waiting_for_key = false;
    pong_state->rebind_key = 0;
    pong_state->selected_key_index = 0;

    pong_state->selected_settings_section = SETTINGS_VICTORY_POINTS;
}

void PongUpdate(PongGameState *state) {
    PongGameState *pong_state = (PongGameState*)state;

    if(IsWindowResized()) {
        // Keep player paddle on left side (x stays at 20)
        // Update opponent paddle to stay on right side
        pong_state->oponent_paddle.x = GetScreenWidth() - 40;
        
        // Adjust paddle Y position to suite new screen bounds
        if(pong_state->player_paddle.y + pong_state->player_paddle.height > GetScreenHeight()) {
            pong_state->player_paddle.y = GetScreenHeight() - pong_state->player_paddle.height;
        }
        if(pong_state->oponent_paddle.y + pong_state->oponent_paddle.height > GetScreenHeight()) {
            pong_state->oponent_paddle.y = GetScreenHeight() - pong_state->oponent_paddle.height;
        }
        
        // Adjust ball position to new screen bounds
        if(pong_state->ball_position.x > GetScreenWidth()) {
            pong_state->ball_position.x = GetScreenWidth() - 20;
        }
        if(pong_state->ball_position.y > GetScreenHeight()) {
            pong_state->ball_position.y = GetScreenHeight() - 20;
        }
    }

    if(pong_state->waiting_for_key) {
        for(int i = 32; i < 256; i++) {
            if(IsKeyPressed(i)) {
                switch(pong_state->rebind_key) {
                    case 1: pong_state->key_player_up = i; break;
                    case 2: pong_state->key_player_down = i; break;
                    case 3: pong_state->key_opponent_up = i; break;
                    case 4: pong_state->key_opponent_down = i; break;
                }
                pong_state->waiting_for_key = false;
                break;
            }
        }
        // To cancle rebinding
        if(IsKeyPressed(KEY_ESCAPE)) {
            pong_state->waiting_for_key = false;
        }
    }

    if(IsKeyPressed(KEY_ESCAPE)) {
        if(pong_state->current_screen == GAMEPLAY) {
            pong_state->current_screen = PAUSED;
        } else if(pong_state->current_screen == PAUSED) {
            pong_state->current_screen = GAMEPLAY;
        } else if(pong_state->current_screen == SETTINGS) {
            pong_state->current_screen = pong_state->prev_screen;
        } else if(pong_state->current_screen == MENU) {
            exit(0);
        }
    }

    if(pong_state->current_screen == PAUSED) {
        if(IsKeyPressed(KEY_UP)) {
            pong_state->selected_pause =(pong_state->selected_pause + PAUSE_OPTION_COUT - 1) % PAUSE_OPTION_COUT;
        } else if(IsKeyPressed(KEY_DOWN)) {
            pong_state->selected_pause = (pong_state->selected_pause + 1) % PAUSE_OPTION_COUT;
        } else if(IsKeyPressed(KEY_ENTER)) {
            switch(pong_state->selected_pause) {
                case PAUSE_RESUME: {
                        pong_state->current_screen = GAMEPLAY;
                        break;
                }
                case PAUSE_RESTART: {
                        bool vs_computer = pong_state->vs_computer;
                        PongInit(state);
                        pong_state->vs_computer = vs_computer;
                        pong_state->current_screen = GAMEPLAY;
                        break;      
                }
                case PAUSE_SETTINGS: {
                        pong_state->prev_screen = PAUSED;
                        pong_state->current_screen = SETTINGS;
                        break;
                }
                case PAUSE_QUIT: {
                        pong_state->current_screen = MENU;
                        PongInit(state);
                        break;        
                }
            }
        }
    }
    
    if(pong_state->current_screen == GAMEPLAY) {
        // Player movement
        float prev_player_y = pong_state->player_paddle.y;
        float prev_oponent_y = pong_state->oponent_paddle.y;
        
        if(IsKeyDown(pong_state->key_player_up) && pong_state->player_paddle.y > 0) {
            pong_state->player_paddle.y -= 7;
        }
        if(IsKeyDown(pong_state->key_player_down) && pong_state->player_paddle.y + pong_state->player_paddle.height < GetScreenHeight()) {
            pong_state->player_paddle.y += 7;
        }
        pong_state->player_paddle_speed = pong_state->player_paddle.y - prev_player_y;

        // Computer movement
        if(pong_state->vs_computer) {
            if(pong_state->ball_speed.x > 0) { // React only if ball going towords oponent paddle
                float error = (float)GetRandomValue(-10, 10);
                float target = pong_state->ball_position.y - pong_state->oponent_paddle.height/2 + error;
                // Position where the paddle wants to get (aligns paddle with ball centre plus the offset)

                // Paddle movement towwords target
                if(pong_state->oponent_paddle.y < target) {
                    pong_state->oponent_paddle.y += pong_state->computer_movement_speed;
                } else if(pong_state->oponent_paddle.y > target) {
                    pong_state->oponent_paddle.y -= pong_state->computer_movement_speed;
                }
            }
        } else {
            if (IsKeyDown(pong_state->key_opponent_up) && pong_state->oponent_paddle.y > 0) {
                pong_state->oponent_paddle.y -= 7;
            }
            if (IsKeyDown(pong_state->key_opponent_down) && pong_state->oponent_paddle.y + pong_state->oponent_paddle.height < GetScreenHeight()) {
                pong_state->oponent_paddle.y += 7;
            }
        }
        pong_state->oponent_paddle_speed = pong_state->oponent_paddle.y - prev_oponent_y;

        // Ball movement
        pong_state->ball_position.x += pong_state->ball_speed.x;
        pong_state->ball_position.y += pong_state->ball_speed.y;

        // Ball collision with Top and Botom
        if (pong_state->ball_position.y <= 0 || pong_state->ball_position.y >= GetScreenHeight()) {
            pong_state->ball_speed.y *= -1;

            // Randomize a bit the bouncing from colliding with the wall
            float randomFactor = ((float)GetRandomValue(-30, 30) / 100.0f);
            pong_state->ball_position.x += randomFactor;
            
            PlaySound(pong_state->wall_hit_sound);
        }

        // Ball collision with Paddle
        if (CheckCollisionCircleRec(pong_state->ball_position, 10, pong_state->player_paddle) ||
            CheckCollisionCircleRec(pong_state->ball_position, 10, pong_state->oponent_paddle)) {

            // Calculate relative intersection point (0.0 1.0)
            Rectangle *paddle = CheckCollisionCircleRec(pong_state->ball_position, 10, pong_state->player_paddle) ? &pong_state->player_paddle : &pong_state->oponent_paddle;
            float relative_y_intersection = (pong_state->ball_position.y - paddle->y) / paddle->height;

            float paddle_speed = (paddle == &pong_state->player_paddle) ? pong_state->player_paddle_speed : pong_state->oponent_paddle_speed;

            /* Adjust ball speed based on where it hit
               The middle of the paddle does not change the y of balls trajectory
               Top and bottom of paddle increase y speed */

            pong_state->ball_speed.y = (relative_y_intersection - 0.5) * 10 + paddle_speed * 0.2; // Scale factor of the bounce angle

            pong_state->ball_speed.x *= -1.0; // Just reverse movement

            if(fabsf(paddle_speed) > 0.1) { // If paddle is moving
                pong_state->ball_speed.x *= 1.1; // Increse by 10% speed
            } else { // If paddle is static
                pong_state->ball_speed.x *= 0.90; // Decrease by 10% speed
            }

            PlaySound(pong_state->paddle_hit_sound);
        }

        // Scoring
        if (pong_state->ball_position.x <= 0) {
            pong_state->oponent_score++;
            pong_state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
            pong_state->ball_speed = (Vector2){5, 3};
            PlaySound(pong_state->score_sound);
        } else if (pong_state->ball_position.x >= GetScreenWidth()) {
            pong_state->player_score++;
            pong_state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
            pong_state->ball_speed = (Vector2){-5, 3};
            PlaySound(pong_state->score_sound);
        }

        // Check for Victory
        if(pong_state->player_score >= pong_state->victory_points) {
            pong_state->victory_player = 0;
            pong_state->victory_timer = 240; // 2s at 120fps
            pong_state->current_screen = VICTORY;
        } else if(pong_state->oponent_score >= pong_state->victory_points) {
            pong_state->victory_player = 1;
            pong_state->victory_timer = 240;
            pong_state->current_screen = VICTORY;
        }
    }
}

void PongDraw(PongGameState *state) {
    PongGameState *pong_state = (PongGameState*)state;

    switch(pong_state->current_screen) {
        case MENU: {
                DrawText("PONG", GetScreenWidth()/2 - MeasureText("PONG", 60)/2, GetScreenHeight()/4, 60, WHITE);

                // Draw options
                DrawText("Press 1 for PvP", GetScreenWidth()/2 - MeasureText("Press 1 for PvP", 30)/2, GetScreenHeight()/2, 30, WHITE);
                DrawText("Press 2 for PvE", GetScreenWidth()/2 - MeasureText("Press 2 for PvE", 30)/2, GetScreenHeight()/2 + 40, 30, WHITE);
                DrawText("Press S for Settings", GetScreenWidth()/2 - MeasureText("Press S for Settings", 30)/2, GetScreenHeight()/2 + 80, 30, WHITE);
                DrawText("Press ESC to Exit", GetScreenWidth()/2 - MeasureText("Press ESC to Exit", 30)/2, GetScreenHeight()/2 + 120, 30, WHITE);
                if (IsKeyPressed(KEY_ONE)) {
                    pong_state->vs_computer = false;
                    pong_state->current_screen = GAMEPLAY;
                } else if (IsKeyPressed(KEY_TWO)) {
                    pong_state->vs_computer = true;
                    pong_state->current_screen = GAMEPLAY;
                } else if (IsKeyPressed(KEY_S)) {
                    pong_state->current_screen = SETTINGS;
                }
                break;
        }
        case GAMEPLAY: {
                // Player paddle texture
                DrawRectangleRec(pong_state->player_paddle, WHITE);
                DrawRectangleRec(pong_state->oponent_paddle, WHITE);

                // Draw ball
                DrawCircleV(pong_state->ball_position, 10, WHITE);
                
                // Draw points
                DrawText(TextFormat("%d", pong_state->player_score), GetScreenWidth()/4, 50, 30, WHITE);
                DrawText(TextFormat("%d", pong_state->oponent_score), 3*GetScreenWidth()/4, 50, 30, WHITE);
                DrawText(TextFormat("%d", GetFPS()), 2*GetScreenWidth()/4, 50, 30, WHITE);
                break;
        }
        case PAUSED: {
                DrawRectangleRec(pong_state->player_paddle, WHITE);
                DrawRectangleRec(pong_state->oponent_paddle, WHITE);
                DrawCircleV(pong_state->ball_position, 10, WHITE);
                DrawText(TextFormat("%d", pong_state->player_score), GetScreenWidth()/4, 50, 30, WHITE);
                DrawText(TextFormat("%d", pong_state->oponent_score), 3*GetScreenWidth()/4, 50, 30, WHITE);

                DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 40)/2, GetScreenHeight()/2 - 100, 40, WHITE);

                // Draw pause menu options
                char *pause_options[PAUSE_OPTION_COUT] = {"Resume", "Restart", "Settings", "Quit to Menu"};
                for (int i = 0; i < PAUSE_OPTION_COUT; i++) {
                    Color color = (i == pong_state->selected_pause) ? RED : WHITE;
                    DrawText(pause_options[i], GetScreenWidth()/2 - MeasureText(pause_options[i], 30)/2, GetScreenHeight()/2 - 20 + i * 40, 30, color);
                }
                break;
        }
        case SETTINGS: {
                DrawText("SETTINGS", GetScreenWidth()/2 - MeasureText("SETTINGS", 40)/2, 50, 40, WHITE);

                int start_y = 150;
                int section_spacing = 120;

                if(!pong_state->waiting_for_key) {
                    // UP/DOWN: Navigate between sections
                    if(IsKeyPressed(KEY_UP)) {
                        pong_state->selected_settings_section = (pong_state->selected_settings_section + SETTINGS_SECTION_COUNT - 1) % SETTINGS_SECTION_COUNT;
                        if(pong_state->selected_settings_section == SETTINGS_CONTROLS) {
                            pong_state->selected_key_index = 0; // Reset to first key when entering controls
                        }
                    }
                    if(IsKeyPressed(KEY_DOWN)) {
                        pong_state->selected_settings_section = (pong_state->selected_settings_section + 1) % SETTINGS_SECTION_COUNT;
                        if(pong_state->selected_settings_section == SETTINGS_CONTROLS) {
                            pong_state->selected_key_index = 0; // Reset to first key when entering controls
                        }
                    }

                    // LEFT/RIGHT: Modify values or navigate keys
                    if(pong_state->selected_settings_section == SETTINGS_VICTORY_POINTS) {
                        if(IsKeyPressed(KEY_LEFT)) {
                            for(int i = 0; i < VICTORY_POINTS_OPTIONS; i++) {
                                if(victory_points_options[i] == pong_state->victory_points) {
                                    pong_state->victory_points = victory_points_options[(i + VICTORY_POINTS_OPTIONS - 1) % VICTORY_POINTS_OPTIONS];
                                    break;
                                }
                            }
                        }
                        if(IsKeyPressed(KEY_RIGHT)) {
                            for(int i = 0; i < VICTORY_POINTS_OPTIONS; i++) {
                                if(victory_points_options[i] == pong_state->victory_points) {
                                    pong_state->victory_points = victory_points_options[(i + 1) % VICTORY_POINTS_OPTIONS];
                                    break;
                                }
                            }
                        }
                    } else if(pong_state->selected_settings_section == SETTINGS_DIFFICULTY) {
                        if(IsKeyPressed(KEY_LEFT) && pong_state->difficulty_level > 0) {
                            pong_state->difficulty_level--;
                            pong_state->computer_movement_speed = difficulty_speeds[pong_state->difficulty_level];
                        }
                        if(IsKeyPressed(KEY_RIGHT) && pong_state->difficulty_level < DIFFICULTY_LEVELS - 1) {
                            pong_state->difficulty_level++;
                            pong_state->computer_movement_speed = difficulty_speeds[pong_state->difficulty_level];
                        }
                    } else if(pong_state->selected_settings_section == SETTINGS_CONTROLS) {
                        if(IsKeyPressed(KEY_LEFT)) {
                            pong_state->selected_key_index = (pong_state->selected_key_index + 3) % 4;
                        }
                        if(IsKeyPressed(KEY_RIGHT)) {
                            pong_state->selected_key_index = (pong_state->selected_key_index + 1) % 4;
                        }
                        // ENTER: Start rebinding selected key
                        if(IsKeyPressed(KEY_ENTER)) {
                            pong_state->waiting_for_key = true;
                            pong_state->rebind_key = pong_state->selected_key_index + 1;
                        }
                    }
                }

                // Point section
                int vp_y = start_y;
                Color vp_title_color = (pong_state->selected_settings_section == SETTINGS_VICTORY_POINTS) ? YELLOW : WHITE;
                DrawText("VICTORY POINTS", GetScreenWidth()/2 - MeasureText("VICTORY POINTS", 30)/2, vp_y, 30, vp_title_color);
                
                // Draw selection indicator
                if(pong_state->selected_settings_section == SETTINGS_VICTORY_POINTS) {
                    DrawText(">", GetScreenWidth()/2 - MeasureText("VICTORY POINTS", 30)/2 - 30, vp_y, 30, YELLOW);
                }

                int vp_options_y = vp_y + 50;
                int total_width = VICTORY_POINTS_OPTIONS * 60 - 10;
                int start_x = GetScreenWidth()/2 - total_width/2;
                
                for(int i = 0; i < VICTORY_POINTS_OPTIONS; i++) {
                    Color color = (victory_points_options[i] == pong_state->victory_points) ? GREEN : GRAY;
                    DrawRectangle(start_x + i * 60, vp_options_y, 40, 40, color);
                    DrawText(TextFormat("%d", victory_points_options[i]), start_x + i * 60 + 10, vp_options_y + 10, 20, BLACK);
                }

                // Dificulty section
                int diff_y = start_y + section_spacing;
                Color diff_title_color = (pong_state->selected_settings_section == SETTINGS_DIFFICULTY) ? YELLOW : WHITE;
                DrawText("DIFFICULTY", GetScreenWidth()/2 - MeasureText("DIFFICULTY", 30)/2, diff_y, 30, diff_title_color);
                
                // Selection indicator
                if(pong_state->selected_settings_section == SETTINGS_DIFFICULTY) {
                    DrawText(">", GetScreenWidth()/2 - MeasureText("DIFFICULTY", 30)/2 - 30, diff_y, 30, YELLOW);
                }

                int diff_options_y = diff_y + 50;
                int diff_total_width = DIFFICULTY_LEVELS * 50 - 10;
                int diff_start_x = GetScreenWidth()/2 - diff_total_width/2;
                
                for(int i = 0; i < DIFFICULTY_LEVELS; i++) {
                    Color color = (i == pong_state->difficulty_level) ? RED : GRAY;
                    DrawRectangle(diff_start_x + i * 50, diff_options_y, 30, 30, color);
                    DrawText(TextFormat("%d", i + 1), diff_start_x + i * 50 + 8, diff_options_y + 7, 15, WHITE);
                }

                // === CONTROLS SECTION ===
                int controls_y = start_y + section_spacing * 2;
                Color controls_title_color = (pong_state->selected_settings_section == SETTINGS_CONTROLS) ? YELLOW : WHITE;
                DrawText("CONTROLS", GetScreenWidth()/2 - MeasureText("CONTROLS", 30)/2, controls_y, 30, controls_title_color);
                
                // Draw selection indicator
                if(pong_state->selected_settings_section == SETTINGS_CONTROLS) {
                    DrawText(">", GetScreenWidth()/2 - MeasureText("CONTROLS", 30)/2 - 30, controls_y, 30, YELLOW);
                }
                
                // Controls
                const char *controls[4] = {"Player Up", "Player Down", "Opponent Up", "Opponent Down"};
                int keys[4] = {
                    pong_state->key_player_up,
                    pong_state->key_player_down,
                    pong_state->key_opponent_up,
                    pong_state->key_opponent_down
                };

                int controls_options_y = controls_y + 45;
                for(int i = 0; i < 4; i++) {
                    const char *key_name = GetKeyName(keys[i]);
                    char buff[64];
                    snprintf(buff, sizeof(buff), "%s: %s", controls[i], key_name);

                    int text_width = MeasureText(buff, 20);
                    int x_pos = (GetScreenWidth() - text_width) / 2;
                    
                    Color color = WHITE;
                    if(pong_state->waiting_for_key && pong_state->rebind_key == i + 1) {
                        color = RED;
                    } else if(pong_state->selected_settings_section == SETTINGS_CONTROLS && pong_state->selected_key_index == i) {
                        color = YELLOW;
                    }

                    DrawText(buff, x_pos, controls_options_y + i * 35, 20, color);

                    // Draw highlight box for selected key
                    if(pong_state->selected_settings_section == SETTINGS_CONTROLS && pong_state->selected_key_index == i && !pong_state->waiting_for_key) {
                        DrawRectangleLines(x_pos - 5, controls_options_y + i * 35 - 5, text_width + 10, 30, YELLOW);
                    }

                    // Draw highlight for rebinding
                    if(pong_state->waiting_for_key && pong_state->rebind_key == i + 1) {
                        DrawRectangle(x_pos - 10, controls_options_y + i * 35 - 5, text_width + 20, 30, ColorAlpha(RED, 0.3));
                    }
                }

                int instructions_y = GetScreenHeight() - 120;
                if(!pong_state->waiting_for_key) {
                    DrawText("UP/DOWN: Navigate sections", GetScreenWidth()/2 - MeasureText("UP/DOWN: Navigate sections", 18)/2, instructions_y, 18, GRAY);
                    DrawText("LEFT/RIGHT: Change values or select key", GetScreenWidth()/2 - MeasureText("LEFT/RIGHT: Change values or select key", 18)/2, instructions_y + 25, 18, GRAY);
                    DrawText("ENTER: Rebind selected key", GetScreenWidth()/2 - MeasureText("ENTER: Rebind selected key", 18)/2, instructions_y + 50, 18, GRAY);
                } else {
                    DrawText("PRESS A KEY TO BIND (ESC TO CANCEL)", GetScreenWidth()/2 - MeasureText("PRESS A KEY TO BIND (ESC TO CANCEL)", 25)/2, instructions_y + 20, 25, YELLOW);
                }

                DrawText("Press ESC to return", GetScreenWidth()/2 - MeasureText("Press ESC to return", 18)/2, GetScreenHeight() - 30, 18, WHITE);
                break;                                
        }
        case VICTORY: {
                const char *winner = (pong_state->victory_player == 0) ? "PLAYER 1 WINS!" : "PLAYER 2 WINS!";

                DrawText(winner, GetScreenWidth()/2 - MeasureText(winner, 50)/2, GetScreenHeight()/2 - 50, 50, GREEN);

                DrawText("Press M to return to menu", GetScreenWidth()/2 - MeasureText("Press M to return to menu", 30)/2, GetScreenHeight()/2 + 50, 30, WHITE);

                pong_state->victory_timer--;
                if(pong_state->victory_timer <= 0 || IsKeyPressed(KEY_M)) {
                    pong_state->current_screen = MENU;
                    PongInit(state);
                }
                break;        
        }
    }
}

void PongClose(PongGameState *state) {
    PongGameState *pong_state = (PongGameState*)state;
    UnloadSound(pong_state->paddle_hit_sound);
    UnloadSound(pong_state->score_sound);
    UnloadSound(pong_state->wall_hit_sound);
}
