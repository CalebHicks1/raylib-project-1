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
    // free the prev room if it exists
    if (game->roomTiles != NULL)
    {
        free(game->roomTiles);
    }
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

/*
Given a tile, return tile frames to render the 4 corners of the tile
*/
TileCorners getTileFrames(GameState *game, Tile *tile)
{
    // size of the pieces of the tilemap
    int edgeSize = game->tileSize / 4;
    // default
    TileCorners result = {
        {0 * edgeSize, 0 * edgeSize, edgeSize, edgeSize},
        {5 * edgeSize, 0 * edgeSize, edgeSize, edgeSize},
        {0 * edgeSize, 5 * edgeSize, edgeSize, edgeSize},
        {5 * edgeSize, 5 * edgeSize, edgeSize, edgeSize}};
    return result;
}

void drawRoomTiles(GameState *game)
{
    Rectangle sourceRect = {0, 0, 16, 16};
    for (int x = 0; x < game->roomWidth; x++)
    {
        for (int y = 0; y < game->roomHeight; y++)
        {
            // get current tile
            Tile *tile = &GET_TILE(game, x, y);

            // check mouse collision
            // Vector2 MousePos = GetMousePosition();
            // Vector2 MousePosInWorld = GetScreenToWorld2D(MousePos, game->playerCamera->camera);
            // Rectangle tileRec = {tile->position.x, tile->position.y, game->tileSize, game->tileSize};
            // if (CheckCollisionPointRec(MousePosInWorld, tileRec))
            // {
            //     // if the mouse is in this tile, color it red
            //     DrawRectangle(tile->position.x + 1, tile->position.y + 1, game->tileSize - 2, game->tileSize - 2, RED);
            // }
            // draw each corner of the tile
            // only for walls
            Texture2D tileTexture = game->tileTextures[tile->tileType];
            if (tile->tileType == TILE_WALL)
            {
                TileCorners sourceTiles = getTileFrames(game, tile);
                // top left
                Rectangle topLeftSourceRect = sourceTiles.topLeft;
                Rectangle topLeftDestRect = {tile->position.x, tile->position.y, game->tileSize / 2, game->tileSize / 2};
                DrawTexturePro(tileTexture, topLeftSourceRect, topLeftDestRect, (Vector2){0, 0}, 0, WHITE);
                // top right
                Rectangle topRightSourceRect = sourceTiles.topRight;
                Rectangle topRightDestRect = {tile->position.x + game->tileSize / 2, tile->position.y, game->tileSize / 2, game->tileSize / 2};
                DrawTexturePro(tileTexture, topRightSourceRect, topRightDestRect, (Vector2){0, 0}, 0, WHITE);
                // bottom Left
                Rectangle bottomLeftSourceRect = sourceTiles.bottomLeft;
                Rectangle bottomLeftDestRect = {tile->position.x, tile->position.y + game->tileSize / 2, game->tileSize / 2, game->tileSize / 2};
                DrawTexturePro(tileTexture, bottomLeftSourceRect, bottomLeftDestRect, (Vector2){0, 0}, 0, WHITE);
                // bottom Right
                Rectangle bottomRightSourceRect = sourceTiles.bottomRight;
                Rectangle bottomRightDestRect = {tile->position.x + game->tileSize / 2, tile->position.y + game->tileSize / 2, game->tileSize / 2, game->tileSize / 2};
                DrawTexturePro(tileTexture, bottomRightSourceRect, bottomRightDestRect, (Vector2){0, 0}, 0, WHITE);
            }
            else
            {
                Rectangle sourceRect = {0, 0, game->tileSize, game->tileSize};
                Rectangle destRect = {tile->position.x, tile->position.y, game->tileSize, game->tileSize};
                DrawTexturePro(tileTexture, sourceRect, destRect, (Vector2){0, 0}, 0, WHITE);
            }

            // DrawTexturePro allows us to scale the sprite to the dest size
        }
    }
}