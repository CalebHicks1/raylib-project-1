#include "raylib.h"
#include "world.h"
#include "game_state.h"
#include <stdlib.h>
#include "camera.h"
/*
Given a room width/height, generate a tile map for the room and set it as the game's roomTiles
*/
void loadRoomTiles(GameState *game, int roomWidth, int roomHeight)
{
    // TODO: free the prev room
    // free(game->roomTiles);
    game->roomWidth = roomWidth;
    game->roomHeight = roomHeight;
    int tileCount = roomWidth * roomHeight;
    // allocate memory
    game->roomTiles = malloc(tileCount * sizeof(Tile));
    for (int x = 0; x < roomWidth; x++)
    {
        for (int y = 0; y < roomHeight; y++)
        {
            Tile *tile = &GET_TILE(game, x, y);
            tile->tileType = TILE_FLOOR;
            tile->position = (Vector2){x * game->tileSize, y * game->tileSize};
        }
    }
}

void drawRoomTiles(GameState *game)
{
    for (int x = 0; x < game->roomWidth; x++)
    {
        for (int y = 0; y < game->roomHeight; y++)
        {
            // get current tile
            Tile *tile = &GET_TILE(game, x, y);
            // draw border

            Vector2 MousePos = GetMousePosition();
            Vector2 MousePosInWorld = GetScreenToWorld2D(MousePos, game->playerCamera->camera);
            Rectangle tileRec = {tile->position.x, tile->position.y, game->tileSize, game->tileSize};
            // draw rectangle outline
            DrawRectangle(tile->position.x, tile->position.y, game->tileSize, game->tileSize, RAYWHITE);
            // draw inner area
            if (CheckCollisionPointRec(MousePosInWorld, tileRec))
            {
                // if the mouse is in this tile, color it red
                DrawRectangle(tile->position.x + 1, tile->position.y + 1, game->tileSize - 2, game->tileSize - 2, RED);
            }
            else
            {
                // otherwise, use tile type for color
                DrawRectangle(tile->position.x + 1, tile->position.y + 1, game->tileSize - 2, game->tileSize - 2, GetTileColor(tile->tileType));
            }
        }
    }
}