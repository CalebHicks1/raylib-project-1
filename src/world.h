#ifndef WORLD_H_
#define WORLD_H_

#include "raylib.h"
#include "game_state.h"

// Forward declarations

// Structs
typedef enum TileType
{
    TILE_FLOOR = 0,
    TILE_COUNT // Always keep this last - gives us total count
} TileType;

// Tile properties structure
typedef struct TileProperties
{
    TileType type;
    Color color;
    bool isSolid; // can this be collided with
    const char *name;
} TileProperties;

// Static tile definitions
static const TileProperties TILE_DEFINITIONS[TILE_COUNT] = {
    [TILE_FLOOR] = {TILE_FLOOR, DARKGREEN, false, "Grass"}};

// Tile: one tile of the world map
typedef struct Tile
{
    int tileType;
    Vector2 position;

} Tile;

// Functions
void loadRoomTiles(GameState *game, int roomWidth, int roomHeight);
void drawRoomTiles(GameState *game);

// Helper functions
static inline TileProperties GetTileProperties(TileType type)
{
    if (type >= 0 && type < TILE_COUNT)
        return TILE_DEFINITIONS[type];
    return TILE_DEFINITIONS[TILE_FLOOR]; // Default fallback
}

static inline bool IsTileTypeSolid(TileType type)
{
    return GetTileProperties(type).isSolid;
}

static inline Color GetTileColor(TileType type)
{
    return GetTileProperties(type).color;
}

#endif