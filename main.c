#include "raylib.h"

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    float speed;
    int size;
} Player;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Calamari");

    Player player = {
        .position = { 100, 100 },
        .velocity = { 0, 0 },
        .speed = 5.0f,
        .size = 20
    };

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_RIGHT)) player.velocity.x = player.speed;
        if (IsKeyPressed(KEY_LEFT)) player.velocity.x = -player.speed;
        if (IsKeyPressed(KEY_UP)) player.velocity.y = -player.speed;
        if (IsKeyPressed(KEY_DOWN)) player.velocity.y = player.speed;

        if (IsKeyUp(KEY_RIGHT) && IsKeyUp(KEY_LEFT)) player.velocity.x = 0;
        if (IsKeyUp(KEY_UP) && IsKeyUp(KEY_DOWN)) player.velocity.y = 0;

        player.position.x += player.velocity.x;
        player.position.y += player.velocity.y;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("move the ball with arrow keys", 10, 10, 20, DARKGRAY);

            DrawCircleV(player.position, 50, MAROON);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
