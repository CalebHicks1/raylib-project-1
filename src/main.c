#include "raylib.h"
#include "raymath.h"
#include "second.h"
#include "stdio.h"

#include "player.h"
#include "camera.h"
#include "game_state.h"

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
    float deltaTime = GetFrameTime();
    // reset player's target velocity
    game->player->playerTargetVelocity = (Vector2){0.0f, 0.0f};
    // read keypresses for player direction
    Vector2 direction = getPlayerDirection();

    game->player->playerTargetVelocity.x = game->player->playerSpeed * direction.x;
    game->player->playerTargetVelocity.y = game->player->playerSpeed * direction.y;
    // update player
    float playerAccelTime = 0.09f;
    game->player->playerVelocity.x = Lerp(game->player->playerVelocity.x, game->player->playerTargetVelocity.x, playerAccelTime);
    game->player->playerVelocity.y = Lerp(game->player->playerVelocity.y, game->player->playerTargetVelocity.y, playerAccelTime);

    game->player->playerPos.x += (game->player->playerVelocity.x * deltaTime);
    game->player->playerPos.y += (game->player->playerVelocity.y * deltaTime);
    // update camera
    float camFollowSpeed = 0.02f;
    game->playerCamera->camPos.x = Lerp(game->playerCamera->camPos.x, game->player->playerPos.x, camFollowSpeed);
    game->playerCamera->camPos.y = Lerp(game->playerCamera->camPos.y, game->player->playerPos.y, camFollowSpeed);
    game->playerCamera->camera.target = game->playerCamera->camPos;
}

void drawGame(GameState *game)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);
    // BeginMode3D(camera);

    // draw anything subject to camera
    BeginMode2D(game->playerCamera->camera);

    // draw player
    DrawRectangle(game->player->playerPos.x, game->player->playerPos.y, game->player->playerSize.x, game->player->playerSize.y, RED);
    // draw rect
    DrawRectangle(50, 50, 50, 50, BLUE);
    EndMode2D();

    // draw ui
    DrawText("This is a raylib example", 10, 40, 20, DARKGRAY);

    DrawFPS(10, 10);
    // snprintf(testString, 50, "Player Velocity:\n\t%f\n\t%f", game.player->playerVelocity.x, game.player->playerVelocity.y);
    // DrawText(testString, 10, 60, 20, DARKGRAY);

    EndDrawing();
}