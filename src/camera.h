#ifndef CAMERA_H_
#define CAMERA_H_

#include "raylib.h"
#include "game_state.h"

// Structs
typedef struct PlayerCamera
{
    Camera2D camera;
    Vector2 camPos;
} PlayerCamera;

// Functions
void updateCamera(GameState *game);
#endif