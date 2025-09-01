#ifndef PLAYER_H_
#define PLAYER_H_

#include "raylib.h"
#include "game_state.h"

// Structs
typedef struct Player
{
    Vector2 playerPos;
    Vector2 playerSize;
    float playerSpeed;
    Vector2 playerVelocity;
    Vector2 playerTargetVelocity;
} Player;

// Functions
Vector2 getPlayerDirection();
void updatePlayer(GameState *game);

#endif