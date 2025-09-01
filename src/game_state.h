#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"

// Forward declarations
typedef struct Player Player;
typedef struct PlayerCamera PlayerCamera;
typedef struct Tile Tile;

// Structs
typedef struct GameState
{
    Player *player;             // player struct. defined in player.h
    PlayerCamera *playerCamera; // player camera struct. defined in camera.h
    int screenWidth;
    int screenHeight;
    float deltaTime; // time since last frame
    int tileSize;    // length of the side of one tile, in pixels
    Tile *roomTiles; // single 1D array
    int roomWidth;   // width of the current room
    int roomHeight;  // height of the current room
} GameState;

// Helper macro to access tiles like a 2D array
#define GET_TILE(game, x, y) ((game)->roomTiles[(y) * (game)->roomWidth + (x)])
// Functions
void InitGame(GameState *game);
void FreeGame(GameState *game);
void InitPlayer(GameState *game);
void InitCamera(GameState *game);

#endif