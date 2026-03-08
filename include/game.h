#ifndef GAME_H
#define GAME_H

#include <raylib.h>

typedef struct {
    int player_score;
    int oponent_score;
    bool game_over;
} GameState;

void InitGame(GameState *state);
void UpdateGame(GameState *state);
void DrawGame(GameState *state);
void CloseGame(GameState *state);

#endif
