#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"

// Forward declarations
typedef struct Player Player;
typedef struct PlayerCamera PlayerCamera;

// Structs
typedef struct GameState
{
    Player *player;
    PlayerCamera *playerCamera;
    int screenWidth;
    int screenHeight;
} GameState;
// Functions
void InitGame(GameState *game);
void FreeGame(GameState *game);
void InitPlayer(GameState *game);
void InitCamera(GameState *game);

#endif