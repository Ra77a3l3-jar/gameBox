#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <raylib.h>
#include <stdbool.h>

#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 15
#define PADDLE_SPEED 8
#define BALL_RADIUS 6
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_PADD 5
#define BRICK_HEIGHT 20

typedef enum {
    BREAKOUT_MENU,
    BREAKOUT_GAMEPLAY,
    BREAKOUT_PAUSED,
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

    Rectangle bricks[100];
    bool bricks_active[100];
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

    int key_left;
    int key_right;
} BreakoutGameState;

void BreakoutInit(BreakoutGameState *state);
bool BreakoutUpdate(BreakoutGameState *state);
void BreakoutDraw(BreakoutGameState *state);
void BreakoutClose(BreakoutGameState *state);

void BreakoutSetupLevel(BreakoutGameState *state);

#endif
