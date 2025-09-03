#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include "game_state.h"
#include <stdlib.h>
#include <math.h>
#include "player.h"
#include "camera.h"
#include <stdio.h>
typedef struct SightPolygon
{
    Vector2 *points;
    int pointCount;
} SightPolygon;

// Helper function to cast a ray and find intersection
Vector2 castRay(Vector2 origin, Vector2 direction, Edge *edges, int edgeCount, float maxDistance)
{
    Vector2 closest = Vector2Add(origin, Vector2Scale(direction, maxDistance));
    float closestDistance = maxDistance;

    for (int i = 0; i < edgeCount; i++)
    {
        Edge *edge = &edges[i];

        // Line-line intersection using parametric form
        Vector2 edgeDir = Vector2Subtract(edge->end, edge->start);
        Vector2 toStart = Vector2Subtract(edge->start, origin);

        float denominator = direction.x * edgeDir.y - direction.y * edgeDir.x;

        // Lines are parallel
        if (fabs(denominator) < 0.0001f)
            continue;

        float t = (toStart.x * edgeDir.y - toStart.y * edgeDir.x) / denominator;
        float u = (toStart.x * direction.y - toStart.y * direction.x) / denominator;

        // Check if intersection is valid
        if (t > 0 && u >= 0 && u <= 1)
        {
            if (t < closestDistance)
            {
                closestDistance = t;
                closest = Vector2Add(origin, Vector2Scale(direction, t));
            }
        }
    }

    return closest;
}

// // Get all unique angles to cast rays to
// float *getUniqueAngles(Edge *edges, int edgeCount, Vector2 origin, int *angleCount)
// {
//     // Each edge has 2 points, each point gets 3 angles (offset versions)
//     // So we need edgeCount * 2 * 3 = edgeCount * 6 angles
//     float *angles = malloc(edgeCount * 6 * sizeof(float)); // Fix this line
//     *angleCount = 0;

//     for (int i = 0; i < edgeCount; i++)
//     {
//         // Add angle to start point
//         Vector2 toStart = Vector2Subtract(edges[i].start, origin);
//         float angleStart = atan2f(toStart.y, toStart.x);

//         // Add angle to end point
//         Vector2 toEnd = Vector2Subtract(edges[i].end, origin);
//         float angleEnd = atan2f(toEnd.y, toEnd.x);

//         // Add slightly offset angles to handle edge cases
//         angles[(*angleCount)++] = angleStart - 0.0001f;
//         angles[(*angleCount)++] = angleStart;
//         angles[(*angleCount)++] = angleStart + 0.0001f;

//         angles[(*angleCount)++] = angleEnd - 0.0001f;
//         angles[(*angleCount)++] = angleEnd;
//         angles[(*angleCount)++] = angleEnd + 0.0001f;
//     }

//     printf("Generated %d angles from %d edges\n", *angleCount, edgeCount);
//     return angles;
// }

// Helper function to normalize angle to [0, 2π]
float normalizeAngle(float angle)
{
    while (angle < 0)
        angle += 2 * PI;
    while (angle >= 2 * PI)
        angle -= 2 * PI;
    return angle;
}

// Improved comparison function that handles angle wraparound
int compareAngles(const void *a, const void *b)
{
    float fa = normalizeAngle(*(const float *)a);
    float fb = normalizeAngle(*(const float *)b);

    if (fa < fb)
        return -1;
    if (fa > fb)
        return 1;
    return 0;
}

// Structure to hold angle and intersection data together
typedef struct AnglePoint
{
    float angle;
    Vector2 point;
    bool isValid;
} AnglePoint;

// Comparison function for AnglePoint
int compareAnglePoints(const void *a, const void *b)
{
    const AnglePoint *pa = (const AnglePoint *)a;
    const AnglePoint *pb = (const AnglePoint *)b;

    if (pa->angle < pb->angle)
        return -1;
    if (pa->angle > pb->angle)
        return 1;
    return 0;
}

Triangle *calculateSightPolygon(Vector2 origin, Edge *edges, int edgeCount, float maxDistance, GameState *game)
{
    if (game->triangles != NULL)
    {
        free(game->triangles);
    }
    SightPolygon result = {0};

    if (edges == NULL || edgeCount <= 0)
    {
        printf("ERROR: Invalid edges array\n");
        return NULL;
    }

    printf("calculateSightPolygon: origin=(%.1f,%.1f), edgeCount=%d\n", origin.x, origin.y, edgeCount);

    // Get screen corners in world coordinates
    Vector2 screenCorners[4] = {
        GetScreenToWorld2D((Vector2){0, 0}, game->playerCamera->camera),                                  // Top-left
        GetScreenToWorld2D((Vector2){game->screenWidth, 0}, game->playerCamera->camera),                  // Top-right
        GetScreenToWorld2D((Vector2){game->screenWidth, game->screenHeight}, game->playerCamera->camera), // Bottom-right
        GetScreenToWorld2D((Vector2){0, game->screenHeight}, game->playerCamera->camera)                  // Bottom-left
    };

    // Create array to hold angle-point pairs
    int maxPoints = edgeCount * 6 + 4; // Same as before
    AnglePoint *anglePoints = malloc(maxPoints * sizeof(AnglePoint));
    int pointCount = 0;

    // FIRST: Add screen corner points
    for (int i = 0; i < 4; i++)
    {
        Vector2 toCorner = Vector2Subtract(screenCorners[i], origin);
        if (Vector2Length(toCorner) > 0.1f) // Skip if too close
        {
            float angle = atan2f(toCorner.y, toCorner.x);
            Vector2 direction = {cosf(angle), sinf(angle)};

            // Cast ray to this corner (it should hit the screen edge)
            Vector2 intersection = castRay(origin, direction, edges, edgeCount, maxDistance);

            anglePoints[pointCount].angle = normalizeAngle(angle);
            anglePoints[pointCount].point = intersection;
            anglePoints[pointCount].isValid = true;
            pointCount++;
        }
    }

    // Generate rays to edge endpoints with small offsets
    for (int i = 0; i < edgeCount; i++)
    {
        // Process start point
        Vector2 toStart = Vector2Subtract(edges[i].start, origin);
        if (Vector2Length(toStart) > 0.1f) // Skip if too close
        {
            float baseAngle = atan2f(toStart.y, toStart.x);

            // Cast rays with small angular offsets
            float offsets[] = {-0.0001f, 0.0f, 0.0001f};
            for (int j = 0; j < 3; j++)
            {
                float angle = baseAngle + offsets[j];
                Vector2 direction = {cosf(angle), sinf(angle)};
                Vector2 intersection = castRay(origin, direction, edges, edgeCount, maxDistance);

                anglePoints[pointCount].angle = normalizeAngle(angle);
                anglePoints[pointCount].point = intersection;
                anglePoints[pointCount].isValid = true;
                pointCount++;
            }
        }

        // Process end point
        Vector2 toEnd = Vector2Subtract(edges[i].end, origin);
        if (Vector2Length(toEnd) > 0.1f) // Skip if too close
        {
            float baseAngle = atan2f(toEnd.y, toEnd.x);

            // Cast rays with small angular offsets
            float offsets[] = {-0.0001f, 0.0f, 0.0001f};
            for (int j = 0; j < 3; j++)
            {
                float angle = baseAngle + offsets[j];
                Vector2 direction = {cosf(angle), sinf(angle)};
                Vector2 intersection = castRay(origin, direction, edges, edgeCount, maxDistance);

                anglePoints[pointCount].angle = normalizeAngle(angle);
                anglePoints[pointCount].point = intersection;
                anglePoints[pointCount].isValid = true;
                pointCount++;
            }
        }
    }

    // Sort by angle
    qsort(anglePoints, pointCount, sizeof(AnglePoint), compareAnglePoints);

    // draw all lines:
    // for (int i = 0; i < pointCount; i++)
    // {
    //     int scaledAngle = (int)((anglePoints[i].angle / (2.0f * PI)) * 255.0f);
    //     DrawLine(origin.x, origin.y, anglePoints[i].point.x, anglePoints[i].point.y, (Color){scaledAngle, scaledAngle, scaledAngle, 255});
    //     DrawCircle(anglePoints[i].point.x, anglePoints[i].point.y, 2, (Color){scaledAngle, scaledAngle, scaledAngle, 255});
    // }

    // // Remove duplicate points and build final polygon
    AnglePoint *finalAnglePoints = malloc(maxPoints * sizeof(AnglePoint));
    int finalPointCount = 0;

    for (int i = 0; i < pointCount; i++)
    {
        if (!anglePoints[i].isValid)
            continue;

        // Check for duplicates
        bool isDuplicate = false;
        if (finalPointCount > 0)
        {
            Vector2 lastPoint = finalAnglePoints[finalPointCount - 1].point;
            float distance = Vector2Distance(anglePoints[i].point, lastPoint);
            if (distance < 1.0f) // Larger threshold to reduce noise
            {
                isDuplicate = true;
            }
        }

        if (!isDuplicate)
        {
            finalAnglePoints[finalPointCount].point = anglePoints[i].point;
            finalAnglePoints[finalPointCount].angle = anglePoints[i].angle;
            finalPointCount++;
        }
    }

    // printf("Generated sight polygon with %d points from %d angle-points\n", result.pointCount, pointCount);
    // build triangles out of angles. Each triangle should be made up of two angle points, and the origin
    Triangle *triangles = malloc(pointCount * sizeof(Triangle));

    for (int i = 0; i < finalPointCount - 1; i++) // Note the -1 here
    {
        triangles[i].point1 = origin;
        triangles[i].point2 = finalAnglePoints[i].point;
        triangles[i].point3 = finalAnglePoints[i + 1].point;
    }
    // And handle the wraparound case:
    if (finalPointCount > 0)
    {
        triangles[finalPointCount - 1].point1 = origin;
        triangles[finalPointCount - 1].point2 = finalAnglePoints[finalPointCount - 1].point;
        triangles[finalPointCount - 1].point3 = finalAnglePoints[0].point; // Wrap to first point
    }
    game->triangles = triangles;
    game->triangleCount = finalPointCount;

    // draw triangles
    for (int i = 0; i < game->triangleCount; i++)
    {
        float scaledColor = (((float)i / (float)(game->triangleCount)) * 255.0f);
        DrawTriangle(game->triangles[i].point3, game->triangles[i].point2, game->triangles[i].point1, (Color){scaledColor, 255, 255, 255});
        // DrawLine(game->triangles[i].point1.x, game->triangles[i].point1.y, game->triangles[i].point2.x, game->triangles[i].point2.y, WHITE);
        // DrawLine(game->triangles[i].point1.x, game->triangles[i].point1.y, game->triangles[i].point3.x, game->triangles[i].point3.y, WHITE);
        // DrawLine(game->triangles[i].point2.x, game->triangles[i].point2.y, game->triangles[i].point3.x, game->triangles[i].point3.y, WHITE);
        // DrawCircle(game->triangles[i].point2.x, game->triangles[i].point2.x, 5, WHITE);
    }
    // draw final lines
    for (int i = 0; i < finalPointCount; i++)
    {
        float scaledColor = (((float)i / (float)(finalPointCount)) * 255.0f);
        DrawLine(origin.x, origin.y, finalAnglePoints[i].point.x, finalAnglePoints[i].point.y, (Color){scaledColor, 255, 255, 255});

        DrawCircle(finalAnglePoints[i].point.x, finalAnglePoints[i].point.y, 2, (Color){scaledColor, 255, 255, 255});
        DrawText(TextFormat("%d", i),
                 finalAnglePoints[i].point.x + 5,  // Offset slightly to the right
                 finalAnglePoints[i].point.y - 10, // Offset slightly above
                 12,                               // Font size
                 BLACK);                           // Text color
    }

    free(anglePoints);
    free(finalAnglePoints);
    game->triangleCount = finalPointCount;
    return triangles;

    return triangles;
}

// Draw the sight polygon
void drawSightPolygon(GameState *game, Color color)
{
    for (int i = 0; i < game->triangleCount; i++)
    {
        // DrawTriangle(game->triangles[i].point1, game->triangles[i].point2, game->triangles[i].point3, BLUE);
        // DrawLine(game->triangles[i].point1.x, game->triangles[i].point1.y, game->triangles[i].point2.x, game->triangles[i].point2.y, WHITE);
        // DrawLine(game->triangles[i].point1.x, game->triangles[i].point1.y, game->triangles[i].point3.x, game->triangles[i].point3.y, WHITE);
        // DrawLine(game->triangles[i].point2.x, game->triangles[i].point2.y, game->triangles[i].point3.x, game->triangles[i].point3.y, WHITE);
        // DrawCircle(game->triangles[i].point2.x, game->triangles[i].point2.x, 5, WHITE);
    }
    //     if (polygon.pointCount < 3)
    //         return;

    // // Draw as individual triangles from origin
    // Vector2 center = {0, 0}; // You'll need to pass the origin here

    // // For now, use first point as reference
    // Vector2 origin = polygon.points[0]; // This is not ideal - should pass origin separately

    // for (int i = 0; i < polygon.pointCount; i++)
    // {
    //     int next = (i + 1) % polygon.pointCount;
    //     DrawTriangle(origin, polygon.points[i], polygon.points[next], ColorAlpha(color, 0.3f));
    // }

    // // Draw debug lines
    // for (int i = 0; i < polygon.pointCount; i++)
    // {
    //     int next = (i + 1) % polygon.pointCount;
    //     DrawLineV(polygon.points[i], polygon.points[next], RED);
    //     DrawCircleV(polygon.points[i], 2, BLUE);
    // }
}

// Free sight polygon memory
// void freeSightPolygon(SightPolygon *polygon)
// {
//     if (polygon->points != NULL)
//     {
//         free(polygon->points);
//         polygon->points = NULL;
//         polygon->pointCount = 0;
//     }
// }

// Convenience function for game integration
// Convenience function for game integration
Triangle *calculatePlayerSight(GameState *game, float sightRange)
{
    SightPolygon emptyResult = {0};

    printf("calculatePlayerSight: edgeCount=%d, sightRange=%.1f\n", game->roomEdgeCount, sightRange);

    Vector2 playerCenter = {
        game->player->playerPos.x + game->player->playerSize.x / 2,
        game->player->playerPos.y + game->player->playerSize.y / 2};

    return calculateSightPolygon(playerCenter, game->roomEdges, game->roomEdgeCount, sightRange, game);
}