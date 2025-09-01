#include "raylib.h"
#include "raymath.h"
#include "second.h"
#include "stdio.h"

#include "player.h"
#include "camera.h"
#include "game_state.h"
#include "world.h"

void updateGame(GameState *game);
void drawGame(GameState *game);

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    GameState game = {0};
    game.screenHeight = screenHeight;
    game.screenWidth = screenWidth;
    InitWindow(screenWidth, screenHeight, "raylib");
    InitGame(&game);
    SetTargetFPS(144); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        updateGame(&game);
        // Draw
        drawGame(&game);
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context
    FreeGame(&game);

    return 0;
}

void updateGame(GameState *game)
{
    // get time since last frame
    game->deltaTime = GetFrameTime();

    updatePlayer(game);
    // update camera
    updateCamera(game);
}

void drawGame(GameState *game)
{
    BeginDrawing();

    ClearBackground(BLACK);
    // BeginMode3D(camera);

    // draw anything subject to camera
    BeginMode2D(game->playerCamera->camera);

    // draw room tiles
    drawRoomTiles(game);

    // draw player
    DrawRectangle(game->player->playerPos.x, game->player->playerPos.y, game->player->playerSize.x, game->player->playerSize.y, RED);

    // Anything after EndMode2D() will be drawn outside of the camera (Like the UI)
    EndMode2D();

    // draw ui
    DrawText("This is a raylib example", 10, 40, 20, DARKGRAY);

    DrawFPS(10, 10);
    // snprintf(testString, 50, "Player Velocity:\n\t%f\n\t%f", game.player->playerVelocity.x, game.player->playerVelocity.y);
    // DrawText(testString, 10, 60, 20, DARKGRAY);

    EndDrawing();
}