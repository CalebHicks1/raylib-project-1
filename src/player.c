#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include "game_state.h"

/*
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

void updatePlayer(GameState *game)
{
    // reset player's target velocity
    game->player->playerTargetVelocity = (Vector2){0.0f, 0.0f};
    // read keypresses for player direction
    Vector2 direction = getPlayerDirection();

    // update the player's target velocity
    game->player->playerTargetVelocity.x = game->player->playerSpeed * direction.x;
    game->player->playerTargetVelocity.y = game->player->playerSpeed * direction.y;

    // lerp the player's actual velocity towards the target velocity
    float playerAccelTime = 0.09f;
    game->player->playerVelocity.x = Lerp(game->player->playerVelocity.x, game->player->playerTargetVelocity.x, playerAccelTime);
    game->player->playerVelocity.y = Lerp(game->player->playerVelocity.y, game->player->playerTargetVelocity.y, playerAccelTime);

    // update the player's position
    game->player->playerPos.x += (game->player->playerVelocity.x * game->deltaTime);
    game->player->playerPos.y += (game->player->playerVelocity.y * game->deltaTime);
}