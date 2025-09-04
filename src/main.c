#include "raylib.h"
#include "raymath.h"
#include "second.h"
#include "stdio.h"
#include <stdlib.h>
#include "player.h"
#include "camera.h"
#include "game_state.h"
#include "world.h"
#include "ray_casting.h"

void updateGame(GameState *game);
void drawGame(GameState *game, RenderTexture2D, RenderTexture2D shadowTexture, RenderTexture2D worldTexture);

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    // window and game init
    const int screenWidth = 800;
    const int screenHeight = 450;
    GameState game = {0};
    game.screenHeight = screenHeight;
    game.screenWidth = screenWidth;
    InitWindow(screenWidth, screenHeight, "raylib");
    InitGame(&game);
    SetTargetFPS(144); // Set our game to run at 60 frames-per-second
    // shaders
    Shader spotlightShader = LoadShader(0, "resources/shaders/spotlight.fs");
    float lightPosLoc = GetShaderLocation(spotlightShader, "lightPos");
    Vector2 lightPos = {0, 0};
    SetShaderValue(spotlightShader, lightPosLoc, &lightPos, SHADER_UNIFORM_VEC2);
    int resolutionLoc = GetShaderLocation(spotlightShader, "resolution");
    Vector2 resolution = {(float)screenWidth, (float)screenHeight};
    SetShaderValue(spotlightShader, resolutionLoc, &resolution, SHADER_UNIFORM_VEC2);
    // white is light, black is dark
    RenderTexture2D lightTexture = LoadRenderTexture(game.screenWidth, game.screenHeight);
    RenderTexture2D shadowTexture = LoadRenderTexture(game.screenWidth, game.screenHeight);
    RenderTexture2D worldTexture = LoadRenderTexture(game.screenWidth, game.screenHeight);
    SetTextureFilter(lightTexture.texture, TEXTURE_FILTER_BILINEAR);
    game.spotlightShader = spotlightShader;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        updateGame(&game);
        Vector2 playerScreenPos = GetWorldToScreen2D(Vector2Add(game.player->playerPos, Vector2Scale(game.player->playerSize, 0.5)), game.playerCamera->camera);
        SetShaderValue(spotlightShader, lightPosLoc, &playerScreenPos, SHADER_UNIFORM_VEC2);

        // Draw
        drawGame(&game, lightTexture, shadowTexture, worldTexture);
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context
    FreeGame(&game);
    UnloadRenderTexture(lightTexture);
    UnloadRenderTexture(shadowTexture);
    UnloadRenderTexture(worldTexture);

    return 0;
}

void updateGame(GameState *game)
{
    // get time since last frame
    game->deltaTime = GetFrameTime();

    // Handle tile clicking
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mousePos = GetMousePosition();
        Vector2 mousePosInWorld = GetScreenToWorld2D(mousePos, game->playerCamera->camera);

        // Convert world position to tile coordinates
        int tileX = (int)(mousePosInWorld.x / game->tileSize);
        int tileY = (int)(mousePosInWorld.y / game->tileSize);

        // Check if tile coordinates are valid
        if (tileX >= 0 && tileX < game->roomWidth &&
            tileY >= 0 && tileY < game->roomHeight)
        {
            Tile *clickedTile = &GET_TILE(game, tileX, tileY);

            // Toggle between floor and wall
            if (clickedTile->tileType == TILE_FLOOR)
                clickedTile->tileType = TILE_WALL;
            else if (clickedTile->tileType == TILE_WALL)
                clickedTile->tileType = TILE_FLOOR;
            roomTilesToRoomLines(game);
        }
    }

    updatePlayer(game);
    // update camera
    updateCamera(game);
}

void drawGame(GameState *game, RenderTexture2D lightTexture, RenderTexture2D shadowTexture, RenderTexture2D worldTexture)
{

    // Calculate and draw sight polygon
    Triangle *sight = calculatePlayerSight(game, game->screenWidth); // 300 pixel sight range

    BeginTextureMode(shadowTexture);
    DrawRectangle(0, 0, game->screenWidth, game->screenHeight, BLACK);
    EndTextureMode();
    // Make sure your lightTexture is the same size as your screen
    BeginTextureMode(lightTexture);
    // After creating the render texture, set filtering mode
    ClearBackground(BLACK);
    // draw white triangles every where the light can touch
    drawSightPolygon(game, ColorAlpha(YELLOW, 0.3f));
    EndTextureMode();

    BeginTextureMode(worldTexture);
    BeginMode2D(game->playerCamera->camera);
    ClearBackground(BLACK);
    drawRoomTiles(game);

    // draw edge visualizations
    for (int i = 0; i < game->roomEdgeCount; i++)
    {
        Edge currEdge = game->roomEdges[i];
        // DrawCircle(currEdge.start.x, currEdge.start.y, 5, RED);
        // DrawCircle(currEdge.end.x, currEdge.end.y, 5, RED);
        DrawLine(currEdge.start.x, currEdge.start.y, currEdge.end.x, currEdge.end.y, RED);
    }

    // draw player
    DrawRectangle(game->player->playerPos.x, game->player->playerPos.y, game->player->playerSize.x, game->player->playerSize.y, WHITE);
    EndMode2D();
    EndTextureMode();

    // draw texture to screen
    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle source = {0, 0, (float)worldTexture.texture.width, -(float)worldTexture.texture.height};
    Rectangle dest = {0, 0, (float)worldTexture.texture.width, (float)worldTexture.texture.height};
    DrawTexturePro(worldTexture.texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);

    BeginShaderMode(game->spotlightShader);
    SetShaderValueTexture(game->spotlightShader, GetShaderLocation(game->spotlightShader, "lightTexture"), lightTexture.texture);
    // DrawRectangle(0, 0, game->screenWidth, game->screenHeight, WHITE);
    DrawTexture(shadowTexture.texture, 0, 0, BLACK);
    EndShaderMode();

    // draw ui

    DrawText("This is a raylib example", 10, 40, 20, DARKGRAY);

    DrawFPS(10, 10);
    // snprintf(testString, 50, "Player Velocity:\n\t%f\n\t%f", game.player->playerVelocity.x, game.player->playerVelocity.y);
    // DrawText(testString, 10, 60, 20, DARKGRAY);

    EndDrawing();
}