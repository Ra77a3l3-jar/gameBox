#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include "gamebox.h"

typedef enum {
    PAUSE_RESUME,
    PAUSE_RESTART,
    PAUSE_SETTINGS,
    PAUSE_QUIT,
    PAUSE_OPTION_COUT
} PauseOption;

typedef enum {
    SETTINGS_VICTORY_POINTS,
    SETTINGS_DIFFICULTY,
    SETTINGS_CONTROLS,
    SETTINGS_SECTION_COUNT
} SettingsSection;

typedef enum {
    PONG_MENU,
    PONG_GAMEPLAY,
    PONG_PAUSED,
    PONG_SETTINGS,
    PONG_VICTORY
} GameScreen;

typedef struct {
    GameScreen current_screen;
    GameScreen prev_screen;

    Rectangle player_paddle;
    float player_paddle_speed;
    Rectangle oponent_paddle;
    float oponent_paddle_speed;

    int key_player_up;
    int key_player_down;
    int key_opponent_up;
    int key_opponent_down;
    bool waiting_for_key;
    int rebind_key; // Current key being rebinded
    int selected_key_index; // For navigating through keys with left/right

    // Settings navigation
    SettingsSection selected_settings_section;

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

    PauseOption selected_pause;
} PongGameState;

void PongInit(void *state);
bool PongUpdate(void *state);
void PongDraw(void *state);
void PongClose(void *state);

#endif
