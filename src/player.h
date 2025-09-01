#ifndef PLAYER_H_
#define PLAYER_H_

#include "raylib.h"
#include "game_state.h"

// Structs
typedef struct Player
{
    Vector2 playerPos;            // position of the player in the world.
    Vector2 playerSize;           // size of the player rect
    float playerSpeed;            // max walking speed
    Vector2 playerVelocity;       // current velocity
    Vector2 playerTargetVelocity; // target velocity (used to lerp current to max)
} Player;

// Functions
Vector2 getPlayerDirection();
void updatePlayer(GameState *game);

#endif