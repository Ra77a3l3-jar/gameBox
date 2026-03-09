#include <raylib.h>
#include <stdbool.h>
#include "game.h"
#include "stdlib.h"

void InitGame(GameState *state) {
    state->player_paddle = (Rectangle){20, GetScreenHeight()/2 - 50, 20, 100};
    state->oponent_paddle = (Rectangle){GetScreenWidth() - 40, GetScreenHeight()/2 - 50, 20, 100};

    state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
    state->ball_speed = (Vector2){5, 3};
    
    state->player_score = 0;
    state->oponent_score = 0;
    state->game_over = false;
}

void UpdateGame(GameState *state) {
    // Player movements
    if(IsKeyDown(KEY_W) && state->player_paddle.y > 0) state->player_paddle.y -= 7;
    if(IsKeyDown(KEY_D) && state->player_paddle.y + state->player_paddle.height < GetScreenHeight()) state->player_paddle.y += 7;

    // Oponent movements
    if(IsKeyDown(KEY_UP) && state->oponent_paddle.y > 0) state->oponent_paddle.y -= 7;
    if(IsKeyDown(KEY_RIGHT) && state->oponent_paddle.y + state->oponent_paddle.height < GetScreenHeight()) state->oponent_paddle.y += 7;

    // Ball movements
    state->ball_position.x += state->ball_speed.x;
    state->ball_position.y += state->ball_speed.y;
    
    // Ball collisions with Top and Botom
    if(state->ball_position.y <= 0 || state->ball_position.y >= GetScreenHeight()) state->ball_speed.y *= -1;

    // Ball collision with Paddle    
    if(CheckCollisionCircleRec(state->ball_position, 10, state->player_paddle) ||
       CheckCollisionCircleRec(state->ball_position, 10, state->oponent_paddle)) {
        state->ball_speed.x *= -1;
    }

    // Scoring
    if(state->ball_position.x <= 0) {
        state->oponent_score++;
        state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
        state->ball_speed = (Vector2){5, 3};
    } else if(state->ball_position.x >= GetScreenWidth()) {
        state->player_score++;
        state->ball_position = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
        state->ball_speed = (Vector2){-5, 3};
    }
}

void DrawGame(GameState *state) {
    // Player paddle texture
    DrawRectangleRec(state->player_paddle, WHITE);
    DrawRectangleRec(state->oponent_paddle, WHITE);

    // Draw ball
    DrawCircleV(state->ball_position, 10, WHITE);

    // Draw points
    DrawText(TextFormat("%d", state->player_score), GetScreenWidth()/4, 50, 30, WHITE);
    DrawText(TextFormat("%d", state->oponent_score), 3*GetScreenWidth()/4, 50, 30, WHITE);
}

void CloseGame(GameState *state) {
    
}
