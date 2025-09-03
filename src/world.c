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
typedef enum Direction
{
    DIRECTION_NORTH = 0,
    DIRECTION_EAST = 1,
    DIRECTION_SOUTH = 2,
    DIRECTION_WEST = 3
} Direction;
/*
    0 = north
    1 = east
    2 = sout
    3 = west
*/
bool hasNeighbor(GameState *game, Tile *tile, Direction direction)
{
    // Get tile coordinates
    int tileX = (int)(tile->position.x / game->tileSize);
    int tileY = (int)(tile->position.y / game->tileSize);

    // Calculate neighbor coordinates
    int neighborX = tileX;
    int neighborY = tileY;

    switch (direction)
    {
    case DIRECTION_NORTH:
        neighborY -= 1;
        break;
    case DIRECTION_EAST:
        neighborX += 1;
        break;
    case DIRECTION_SOUTH:
        neighborY += 1;
        break;
    case DIRECTION_WEST:
        neighborX -= 1;
        break;
    }

    // Check bounds
    if (neighborX < 0 || neighborX >= game->roomWidth ||
        neighborY < 0 || neighborY >= game->roomHeight)
    {
        return false; // Out of bounds
    }

    // Check if neighbor tile exists and is the same type
    Tile *neighborTile = &GET_TILE(game, neighborX, neighborY);
    return (neighborTile->tileType == tile->tileType);
}

bool hasNeighborDiagonal(GameState *game, Tile *tile, Direction dir1, Direction dir2)
{
    // Get tile coordinates
    int tileX = (int)(tile->position.x / game->tileSize);
    int tileY = (int)(tile->position.y / game->tileSize);

    int neighborX = tileX;
    int neighborY = tileY;

    // Apply both direction offsets
    if (dir1 == DIRECTION_NORTH || dir2 == DIRECTION_NORTH)
        neighborY -= 1;
    if (dir1 == DIRECTION_SOUTH || dir2 == DIRECTION_SOUTH)
        neighborY += 1;
    if (dir1 == DIRECTION_EAST || dir2 == DIRECTION_EAST)
        neighborX += 1;
    if (dir1 == DIRECTION_WEST || dir2 == DIRECTION_WEST)
        neighborX -= 1;

    // Check bounds
    if (neighborX < 0 || neighborX >= game->roomWidth ||
        neighborY < 0 || neighborY >= game->roomHeight)
    {
        return false;
    }

    Tile *neighborTile = &GET_TILE(game, neighborX, neighborY);
    return (neighborTile->tileType == tile->tileType);
}

/*
Given a tile, return tile frames to render the 4 corners of the tile
*/
TileCorners getTileFrames(GameState *game, Tile *tile)
{
    int edgeSize = 16 / 2; // tileset uses 16 pixels, and each corner of a tile is 8x8

    // Check all 8 neighbors (including diagonals)
    bool hasNorth = hasNeighbor(game, tile, DIRECTION_NORTH);
    bool hasEast = hasNeighbor(game, tile, DIRECTION_EAST);
    bool hasSouth = hasNeighbor(game, tile, DIRECTION_SOUTH);
    bool hasWest = hasNeighbor(game, tile, DIRECTION_WEST);

    // Check diagonal neighbors
    bool hasNorthWest = hasNeighborDiagonal(game, tile, DIRECTION_NORTH, DIRECTION_WEST);
    bool hasNorthEast = hasNeighborDiagonal(game, tile, DIRECTION_NORTH, DIRECTION_EAST);
    bool hasSouthWest = hasNeighborDiagonal(game, tile, DIRECTION_SOUTH, DIRECTION_WEST);
    bool hasSouthEast = hasNeighborDiagonal(game, tile, DIRECTION_SOUTH, DIRECTION_EAST);

    TileCorners result;

    // TOP LEFT CORNER
    if (!hasNorth && !hasWest)
    {
        // Isolated corner
        result.topLeft = (Rectangle){0, 0, edgeSize, edgeSize};
    }
    else if (hasNorth && !hasWest)
    {
        // North edge
        result.topLeft = (Rectangle){0, edgeSize, edgeSize, edgeSize};
    }
    else if (!hasNorth && hasWest)
    {
        // West edge
        result.topLeft = (Rectangle){edgeSize, 0, edgeSize, edgeSize};
    }
    else if (hasNorth && hasWest && !hasNorthWest)
    {
        // Inner corner (concave)
        result.topLeft = (Rectangle){4 * edgeSize, 1 * edgeSize, edgeSize, edgeSize};
    }
    else
    {
        // Fully connected
        result.topLeft = (Rectangle){edgeSize, edgeSize, edgeSize, edgeSize};
    }

    // TOP RIGHT CORNER
    if (!hasNorth && !hasEast)
    {
        // Isolated corner
        result.topRight = (Rectangle){2 * edgeSize, 0, edgeSize, edgeSize};
    }
    else if (hasNorth && !hasEast)
    {
        // North edge
        result.topRight = (Rectangle){2 * edgeSize, edgeSize, edgeSize, edgeSize};
    }
    else if (!hasNorth && hasEast)
    {
        // East edge
        result.topRight = (Rectangle){edgeSize, 0, edgeSize, edgeSize};
    }
    else if (hasNorth && hasEast && !hasNorthEast)
    {
        // Inner corner (concave)
        result.topRight = (Rectangle){3 * edgeSize, 1 * edgeSize, edgeSize, edgeSize};
    }
    else
    {
        // Fully connected
        result.topRight = (Rectangle){edgeSize, edgeSize, edgeSize, edgeSize};
    }

    // BOTTOM LEFT CORNER
    if (!hasSouth && !hasWest)
    {
        // Isolated corner
        result.bottomLeft = (Rectangle){0, 2 * edgeSize, edgeSize, edgeSize};
    }
    else if (hasSouth && !hasWest)
    {
        // South edge
        result.bottomLeft = (Rectangle){0, edgeSize, edgeSize, edgeSize};
    }
    else if (!hasSouth && hasWest)
    {
        // West edge
        result.bottomLeft = (Rectangle){edgeSize, 2 * edgeSize, edgeSize, edgeSize};
    }
    else if (hasSouth && hasWest && !hasSouthWest)
    {
        // Inner corner (concave)
        result.bottomLeft = (Rectangle){4 * edgeSize, 0, edgeSize, edgeSize};
    }
    else
    {
        // Fully connected
        result.bottomLeft = (Rectangle){edgeSize, edgeSize, edgeSize, edgeSize};
    }

    // BOTTOM RIGHT CORNER
    if (!hasSouth && !hasEast)
    {
        // Isolated corner
        result.bottomRight = (Rectangle){2 * edgeSize, 2 * edgeSize, edgeSize, edgeSize};
    }
    else if (hasSouth && !hasEast)
    {
        // South edge
        result.bottomRight = (Rectangle){2 * edgeSize, edgeSize, edgeSize, edgeSize};
    }
    else if (!hasSouth && hasEast)
    {
        // East edge
        result.bottomRight = (Rectangle){edgeSize, 2 * edgeSize, edgeSize, edgeSize};
    }
    else if (hasSouth && hasEast && !hasSouthEast)
    {
        // Inner corner (concave)
        result.bottomRight = (Rectangle){3 * edgeSize, 0, edgeSize, edgeSize};
    }
    else
    {
        // Fully connected
        result.bottomRight = (Rectangle){edgeSize, edgeSize, edgeSize, edgeSize};
    }

    return result;
}

void drawRoomTiles(GameState *game)
{
    // actual size of the texture
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
            Texture2D tileTexture = game->tileTextures[TILE_FLOOR];
            Rectangle destRect = {tile->position.x, tile->position.y, game->tileSize, game->tileSize};
            // DrawTexturePro allows us to scale the sprite to the dest size
            DrawTexturePro(tileTexture, sourceRect, destRect, (Vector2){0, 0}, 0, WHITE);
            if (tile->tileType == TILE_WALL)
            {
                Texture2D wallTexture = game->tileTextures[TILE_WALL];
                TileCorners sourceTiles = getTileFrames(game, tile);
                // top left
                Rectangle topLeftSourceRect = sourceTiles.topLeft;
                Rectangle topLeftDestRect = {tile->position.x, tile->position.y, game->tileSize / 2, game->tileSize / 2};
                DrawTexturePro(wallTexture, topLeftSourceRect, topLeftDestRect, (Vector2){0, 0}, 0, WHITE);
                // top right
                Rectangle topRightSourceRect = sourceTiles.topRight;
                Rectangle topRightDestRect = {tile->position.x + game->tileSize / 2, tile->position.y, game->tileSize / 2, game->tileSize / 2};
                DrawTexturePro(wallTexture, topRightSourceRect, topRightDestRect, (Vector2){0, 0}, 0, WHITE);
                // bottom Left
                Rectangle bottomLeftSourceRect = sourceTiles.bottomLeft;
                Rectangle bottomLeftDestRect = {tile->position.x, tile->position.y + game->tileSize / 2, game->tileSize / 2, game->tileSize / 2};
                DrawTexturePro(wallTexture, bottomLeftSourceRect, bottomLeftDestRect, (Vector2){0, 0}, 0, WHITE);
                // bottom Right
                Rectangle bottomRightSourceRect = sourceTiles.bottomRight;
                Rectangle bottomRightDestRect = {tile->position.x + game->tileSize / 2, tile->position.y + game->tileSize / 2, game->tileSize / 2, game->tileSize / 2};
                DrawTexturePro(wallTexture, bottomRightSourceRect, bottomRightDestRect, (Vector2){0, 0}, 0, WHITE);
            }
        }
    }
}

/*
    Given a 1D array of tiles, identify all contiguous edges of each group of tiles
    This greatly reduces the number of points to check when calculating shadows

    For now, just operate on whole room
*/
void roomTilesToRoomLines(GameState *game)
{
    if (game->roomEdges != NULL)
    {
        free(game->roomEdges);
    }

    // used to track which edge each tile is using
    TileEdges *visitedTiles = calloc(game->roomHeight * game->roomWidth, sizeof(TileEdges));
    // TODO: do this better
    // Edge *edges = calloc(200, sizeof(Edge));
    // iterate through all tiles in room
    int edgeIndex = 0;
    for (int x = 0; x < game->roomWidth; x++)
    {
        for (int y = 0; y < game->roomHeight; y++)
        {
            // get array indicies for each neighboring tile
            // can access a 1d array like a 2d array using the formula
            // (y * width) + x
            int i = ((y)*game->roomWidth) + (x);
            int north = ((y - 1) * game->roomWidth) + (x);
            int south = ((y + 1) * game->roomWidth) + (x);
            int east = ((y)*game->roomWidth) + (x + 1);
            int west = ((y)*game->roomWidth) + (x - 1);

            Tile thisTile = game->roomTiles[i];
            // initialize tileEdges for this tile
            visitedTiles[i].isWall = false;
            visitedTiles[i].northEdgeId = -1;
            visitedTiles[i].southEdgeId = -1;
            visitedTiles[i].eastEdgeId = -1;
            visitedTiles[i].westEdgeId = -1;
            // if this tile is a wall, calculate its edges
            if (thisTile.tileType == TILE_WALL)
            {
                visitedTiles[i].isWall = true;
                // does this tile have a western neighbor? if not, get a new western edge
                // check if it is out of bounds first
                if (west < 0 || game->roomTiles[west].tileType != TILE_WALL)
                {
                    // if the tile has a northern neighbor with a western edge, can use its western edge
                    if (north > 0 && game->roomTiles[north].tileType == TILE_WALL && visitedTiles[north].westEdgeId != -1)
                    {

                        // the northern neighbor has a western edge, which we can use now
                        visitedTiles[i].westEdgeId = visitedTiles[north].westEdgeId;
                        // extend the edge down
                    }
                    else
                    {
                        // create a new western edge
                        visitedTiles[i].westEdgeId = edgeIndex;
                        // set the edge start point
                        edgeIndex++;
                    }
                }
                // does this tile have a eastern neighbor? if not, get a new eastern edge
                // check if it is out of bounds first
                if (east > game->roomWidth * game->roomHeight || game->roomTiles[east].tileType != TILE_WALL)
                {
                    // if the tile has a northern neighbor with a eastern edge, can use its eastern edge
                    if (north > 0 && game->roomTiles[north].tileType == TILE_WALL && visitedTiles[north].eastEdgeId != -1)
                    {

                        // the northern neighbor has a eastern edge, which we can use now
                        visitedTiles[i].eastEdgeId = visitedTiles[north].eastEdgeId;
                        // extend the edge down
                    }
                    else
                    {
                        // create a new eastern edge
                        visitedTiles[i].eastEdgeId = edgeIndex;
                        // set the edge start point
                        edgeIndex++;
                    }
                }
                // does this tile have a northern neighbor? if not, get a new northern edge
                // check if it is out of bounds first
                if (north < 0 || game->roomTiles[north].tileType != TILE_WALL)
                {
                    // if the tile has a western neighbor with a western edge, can use its northern edge
                    if (west > 0 && game->roomTiles[west].tileType == TILE_WALL && visitedTiles[west].northEdgeId != -1)
                    {

                        // the western neighbor has a northern edge, which we can use now
                        visitedTiles[i].northEdgeId = visitedTiles[west].northEdgeId;
                        // extend the edge down
                    }
                    else
                    {
                        // create a new northern edge
                        visitedTiles[i].northEdgeId = edgeIndex;
                        // set the edge start point
                        edgeIndex++;
                    }
                }
                // does this tile have a southern neighbor? if not, get a new southern edge
                // check if it is out of bounds first
                if (south > game->roomWidth * game->roomHeight || game->roomTiles[south].tileType != TILE_WALL)
                {
                    // if the tile has a western neighbor with a southern edge, can use its southern edge
                    if (west > 0 && game->roomTiles[west].tileType == TILE_WALL && visitedTiles[west].southEdgeId != -1)
                    {

                        // the western neighbor has a western edge, which we can use now
                        visitedTiles[i].southEdgeId = visitedTiles[west].southEdgeId;
                        // extend the edge down
                    }
                    else
                    {
                        // create a new western edge
                        visitedTiles[i].southEdgeId = edgeIndex;
                        // set the edge start point
                        edgeIndex++;
                    }
                }
            }
        }
    }
    // TODO: build the list of edges now
    // iterate back through the list of visited tiles, extending them as we go
    Edge *edges = calloc(edgeIndex + 1, sizeof(Edge));

    for (int x = 0; x < game->roomWidth; x++)
    {
        for (int y = 0; y < game->roomHeight; y++)
        {
            // calculate each edge's start point, or increase its end point
            int i = ((y)*game->roomWidth) + (x);
            // north edge
            if (visitedTiles[i].northEdgeId != -1)
            {
                if (edges[visitedTiles[i].northEdgeId].visited)
                {
                    // update endpoints of edge
                    edges[visitedTiles[i].northEdgeId].end.x = game->roomTiles[i].position.x + game->tileSize;
                    edges[visitedTiles[i].northEdgeId].end.y = game->roomTiles[i].position.y;
                }
                else
                {
                    // populate start points of edge
                    edges[visitedTiles[i].northEdgeId].visited = true;
                    edges[visitedTiles[i].northEdgeId].start.x = game->roomTiles[i].position.x;
                    edges[visitedTiles[i].northEdgeId].start.y = game->roomTiles[i].position.y;
                    edges[visitedTiles[i].northEdgeId].end.x = game->roomTiles[i].position.x + game->tileSize;
                    edges[visitedTiles[i].northEdgeId].end.y = game->roomTiles[i].position.y;
                }
            }
            // south edge
            if (visitedTiles[i].southEdgeId != -1)
            {

                if (edges[visitedTiles[i].southEdgeId].visited)
                {
                    // update endpoints of edge
                    edges[visitedTiles[i].southEdgeId].end.x = game->roomTiles[i].position.x + game->tileSize;
                    edges[visitedTiles[i].southEdgeId].end.y = game->roomTiles[i].position.y + game->tileSize;
                }
                else
                {
                    // populate start points of edge
                    edges[visitedTiles[i].southEdgeId].visited = true;
                    edges[visitedTiles[i].southEdgeId].start.x = game->roomTiles[i].position.x;
                    edges[visitedTiles[i].southEdgeId].start.y = game->roomTiles[i].position.y + game->tileSize;
                    edges[visitedTiles[i].southEdgeId].end.x = game->roomTiles[i].position.x + game->tileSize;
                    edges[visitedTiles[i].southEdgeId].end.y = game->roomTiles[i].position.y + game->tileSize;
                }
            }
            // east edge
            if (visitedTiles[i].eastEdgeId != -1)
            {

                if (edges[visitedTiles[i].eastEdgeId].visited)
                {
                    // update endpoints of edge
                    edges[visitedTiles[i].eastEdgeId].end.x = game->roomTiles[i].position.x + game->tileSize;
                    edges[visitedTiles[i].eastEdgeId].end.y = game->roomTiles[i].position.y + game->tileSize;
                }
                else
                {
                    // populate start points of edge
                    edges[visitedTiles[i].eastEdgeId].visited = true;
                    edges[visitedTiles[i].eastEdgeId].start.x = game->roomTiles[i].position.x + game->tileSize;
                    edges[visitedTiles[i].eastEdgeId].start.y = game->roomTiles[i].position.y;
                    edges[visitedTiles[i].eastEdgeId].end.x = game->roomTiles[i].position.x + game->tileSize;
                    edges[visitedTiles[i].eastEdgeId].end.y = game->roomTiles[i].position.y + game->tileSize;
                }
            }
            // west edge
            if (visitedTiles[i].westEdgeId != -1)
            {
                if (edges[visitedTiles[i].westEdgeId].visited)
                {
                    // update endpoints of edge
                    edges[visitedTiles[i].westEdgeId].end.x = game->roomTiles[i].position.x;
                    edges[visitedTiles[i].westEdgeId].end.y = edges[visitedTiles[i].westEdgeId].end.y + game->tileSize;
                }
                else
                {
                    // populate start points of edge
                    edges[visitedTiles[i].westEdgeId].visited = true;
                    edges[visitedTiles[i].westEdgeId].start.x = game->roomTiles[i].position.x;
                    edges[visitedTiles[i].westEdgeId].start.y = game->roomTiles[i].position.y;
                    edges[visitedTiles[i].westEdgeId].end.x = game->roomTiles[i].position.x;
                    edges[visitedTiles[i].westEdgeId].end.y = game->roomTiles[i].position.y + game->tileSize;
                }
            }
        }
    }
    game->roomEdges = edges;
    game->roomEdgeCount = edgeIndex;
    free(visitedTiles);
}