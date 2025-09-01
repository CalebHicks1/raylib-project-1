#include "game_state.h"
#include "raylib.h"
#include "camera.h"
#include "player.h"
#include <stdlib.h>
#include "world.h"

void InitGame(GameState *game)
{
    // *game = (GameState){0};
    // init player before camera, as the camera requires some player info
    InitPlayer(game);
    InitCamera(game);
    game->tileSize = 32;
    loadRoomTiles(game, 32, 32);
}

void FreeGame(GameState *game)
{
    free(game->playerCamera);
    free(game->player);
    free(game->roomTiles);
}

void InitCamera(GameState *game)
{
    game->playerCamera = malloc(sizeof(PlayerCamera));
    Camera2D camera = {0};
    game->playerCamera->camera = camera;
    game->playerCamera->camPos = (Vector2){0.0f, 0.0f};
    // center the camera on the player
    game->playerCamera->camera.offset = (Vector2){(game->screenWidth / 2) - (game->player->playerSize.x / 2), (game->screenHeight / 2) - (game->player->playerSize.y / 2)};
    game->playerCamera->camera.zoom = 1;
}

void InitPlayer(GameState *game)
{
    game->player = malloc(sizeof(Player));

    game->player->playerPos = (Vector2){0.0f, 0.0f};
    game->player->playerSpeed = 200.0f;
    game->player->playerVelocity = (Vector2){0.0f, 0.0f};
    game->player->playerTargetVelocity = (Vector2){0.0f, 0.0f};
    game->player->playerSize = (Vector2){50, 50};
}