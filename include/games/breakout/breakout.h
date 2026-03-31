#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <raylib.h>
#include <stdbool.h>

#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 15
#define PADDLE_SPEED 8
#define BALL_RADIUS 6

#define BRICK_NUM 100
#define BRICK_MAX_ROWS 6
#define BRICK_ROWS 5
#define BRICK_MAX_COLS 15
#define BRICK_COLS 10
#define BRICK_PADD 5
#define BRICK_HEIGHT 20

#define LIVES_MAX 9

#define VICTORY_SCREEN_DURATION 600 // 5 seconds at 120fps
#define GAMEOVER_SCREEN_DURATION 600

typedef enum {
    BREAKOUT_PAUSE_RESUME,
    BREAKOUT_PAUSE_RESTART,
    BREAKOUT_PAUSE_SETTINGS,
    BREAKOUT_PAUSE_QUIT,
    BREAKOUT_PAUSE_OPTION_COUNT
} BreakoutPauseOption;

typedef enum {
    BREAKOUT_SETTINGS_PADDLE_WIDTH,
    BREAKOUT_SETTINGS_ROWS,
    BREAKOUT_SETTINGS_COLS,
    BREAKOUT_SETTINGS_LIVES,
    BREAKOUT_SETTINGS_LEVEL,
    BREAKOUT_SETTINGS_BALL_SIZE,
    BREAKOUT_SETTINGS_SECTION_COUNT
} BreakoutSettingsSection;

typedef enum {
    BREAKOUT_MENU,
    BREAKOUT_GAMEPLAY,
    BREAKOUT_PAUSED,
    BREAKOUT_SETTINGS,
    BREAKOUT_VICTORY,
    BREAKOUT_GAME_OVER,
} BreakoutScreen;

typedef struct {
    BreakoutScreen current_screen;
    BreakoutScreen prev_screen;

    Rectangle paddle;
    float paddle_speed;
    int paddle_width;

    Vector2 ball_position;
    Vector2 ball_speed;
    float ball_rad;
    bool ball_active;
    float ball_speed_multiplier;

    Rectangle bricks[BRICK_NUM];
    bool bricks_active[BRICK_NUM];
    int brick_count;
    int brick_rows;
    int brick_cols;
    float brick_width;
    float brick_height;

    int score;
    int level;
    int lives;
    bool game_over;
    bool victory;
    int victory_timer;
    int game_over_timer;

    int key_left;
    int key_right;

    Sound paddle_sound;
    Sound brick_sound;
    Sound wall_hit_sound;
    Sound lose_life_sound;
    Sound game_over_sound;
    Sound victory_sound;

    BreakoutPauseOption selected_pause;
    BreakoutSettingsSection selected_settings_section;
} BreakoutGameState;

void BreakoutInit(BreakoutGameState *state);
bool BreakoutUpdate(BreakoutGameState *state);
void BreakoutDraw(BreakoutGameState *state);
void BreakoutClose(BreakoutGameState *state);

void BreakoutSetupLevel(BreakoutGameState *state);

#endif
