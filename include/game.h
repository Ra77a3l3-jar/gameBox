#ifndef GAME_H
#define GAME_H

#include <raylib.h>

typedef enum {
    MENU,
    GAMEPLAY,
    PAUSED,
    SETTINGS,
    VICTORY
} GameScreen;

typedef struct {
    GameScreen current_screen;
    Rectangle player_paddle;
    Rectangle oponent_paddle;
    Vector2 ball_position;
    Vector2 ball_speed;
    int player_score;
    int oponent_score;
    bool game_over;
    bool vs_computer;
    Sound paddle_hit_sound;
    Sound score_sound;
    Sound wall_hit_sound;

    // Settings
    int difficulty_level; // 1-5, ULTIMATE=6
    int victory_points; // 5, 10, 15, 20
    float computer_movement_speed;

    int victory_timer;
    int victory_player; // 0 Player, 1 Opponent
} GameState;

void InitGame(GameState *state);
void UpdateGame(GameState *state);
void DrawGame(GameState *state);
void CloseGame(GameState *state);

#endif
