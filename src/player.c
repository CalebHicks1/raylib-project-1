#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include "game_state.h"

/**
 * Reads keypresses for player direction. Normalizes the vector before returning
 */
Vector2 getPlayerDirection()
{
    Vector2 direction = (Vector2){0.0f, 0.0f};
    //----------------------------------------------------------------------------------
    // keybinds
    if (IsKeyDown(KEY_D))
    {
        direction.x = 1;
    }
    if (IsKeyDown(KEY_A))
    {
        direction.x = -1;
    }
    if (IsKeyDown(KEY_W))
    {
        direction.y = -1;
    }
    if (IsKeyDown(KEY_S))
    {
        direction.y = 1;
    }

    float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
    // this is so diagonal movement is the same as horizontal/vertical
    if (magnitude > 0)
    {
        direction.x /= magnitude;
        direction.y /= magnitude;
    }

    return direction;
}