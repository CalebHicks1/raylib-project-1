#include "raylib.h"
#include "raymath.h"
#include "game_state.h"
#include "camera.h"
#include "player.h"

void updateCamera(GameState *game)
{
    // How fast does the camera follow the player
    float camFollowSpeed = 0.02f;
    // lerp the camera to the player position
    game->playerCamera->camPos.x = Lerp(game->playerCamera->camPos.x, game->player->playerPos.x, camFollowSpeed);
    game->playerCamera->camPos.y = Lerp(game->playerCamera->camPos.y, game->player->playerPos.y, camFollowSpeed);
    // set the camera's tartget to the new camPos
    game->playerCamera->camera.target = game->playerCamera->camPos;
}