#include "raylib.h"
#include <stdlib.h>

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    float speed;
    int size;
    bool focused;
    float energy;
    float drain;
    float maxEnergy;
} Player;

#define SUMI_PAPER (Color){ 240, 234, 214, 255 }

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    Vector2* path = malloc(sizeof(Vector2) * 25);
    int pathI = 0;
    double lastPath = GetTime();

    InitWindow(screenWidth, screenHeight, "Calamari");

    Player player = {
        .position = { 100, 100 },
        .velocity = { 0, 0 },
        .speed = 5.0f,
        .size = 10,
        .focused = false,
        .energy = 100.0f,
        .drain = 1.0f,
        .maxEnergy = 100.0f
    };

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        double now = GetTime();
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_RIGHT)) player.velocity.x = player.speed;
        if (IsKeyPressed(KEY_LEFT)) player.velocity.x = -player.speed;
        if (IsKeyPressed(KEY_UP)) player.velocity.y = -player.speed;
        if (IsKeyPressed(KEY_DOWN)) player.velocity.y = player.speed;

        if (IsKeyUp(KEY_RIGHT) && IsKeyUp(KEY_LEFT)) player.velocity.x = 0;
        if (IsKeyUp(KEY_UP) && IsKeyUp(KEY_DOWN)) player.velocity.y = 0;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            
            if (mousePos.x >= player.position.x - player.size && mousePos.x <= player.position.x + player.size &&
                mousePos.y >= player.position.y - player.size && mousePos.y <= player.position.y + player.size) {
                player.focused = !player.focused; // Toggle focus state
            }
        }

        player.position.x += player.velocity.x;
        player.position.y += player.velocity.y;

        if (player.focused) {
            if (now - lastPath > 0.05 && pathI < 25) { // Update path every 0.5 seconds
                Vector2 mousePos = GetMousePosition();
                path[pathI++] = (Vector2){ mousePos.x, mousePos.y };
                lastPath = now;
            }

            player.energy -= player.drain;
            if (player.energy < 0) {
                player.energy = 0;
                pathI = 0;
                player.focused = false; // Lose focus when energy depletes
            }
        } else {
            player.energy += player.drain * 0.5f; // Regenerate energy when not focused
            if (player.energy > player.maxEnergy) {
                player.energy = player.maxEnergy;
            }
        }

        if (IsKeyPressed(KEY_H))
        {
            if (IsCursorHidden()) ShowCursor();
            else HideCursor();
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(SUMI_PAPER);

            DrawText("move the ball with arrow keys", 10, 10, 20, DARKGRAY);

            DrawCircleV(player.position, 20, player.focused ? RED : BLUE);

            if (player.focused) {
                for (int i = 0; i < pathI; i++) {
                    DrawCircleV(path[i], 5, BLACK);
                    if (i > 0) {
                        DrawLineV(path[i - 1], path[i], BLACK);
                    }
                }
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
