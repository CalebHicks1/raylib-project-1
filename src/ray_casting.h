#ifndef RAY_CASTING_H
#define RAY_CASTING_H

#include "raylib.h"
#include "game_state.h"

typedef struct SightPolygon
{
    Vector2 *points;
    int pointCount;
} SightPolygon;

// Core functions
Triangle *calculateSightTriangles(Vector2 origin, Edge *edges, int edgeCount, float maxDistance, GameState *game);
Triangle *calculatePlayerSight(GameState *game, float sightRange);

// Utility functions
void drawSightPolygon(GameState *game, Color color);
void freeSightPolygon(SightPolygon *polygon);

#endif