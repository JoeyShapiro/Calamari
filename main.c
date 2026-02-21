#include "raylib.h"
#include <stdlib.h>
#include <math.h>

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    float speed;
    int size;
    bool focused;
    bool dashing;
    float energy;
    float drain;
    float maxEnergy;
    float health;
    float maxHealth;
} Player;

typedef struct Mob {
    Vector2 position;
    Vector2 velocity;
    float speed;
    int size;
    float health;
    float maxHealth;
    float damage;
} Mob;

#define SUMI_PAPER (Color){ 240, 234, 214, 255 }

float catmul_rom(float p0, float p1, float p2, float p3, float t) {
    return 0.5f * ((2.0f * p1) +
                   (-p0 + p2) * t +
                   (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
                   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
}

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
    int curI = 0;
    float t = 0;
    double lastPath = GetTime();

    Mob* mobs = malloc(sizeof(Mob) * 10);
    int mobCount = 0;
    double lastMob = GetTime();

    InitWindow(screenWidth, screenHeight, "Calamari");

    Player player = {
        .position = { 100, 100 },
        .velocity = { 0, 0 },
        .speed = 5.0f,
        .size = 10,
        .focused = false,
        .energy = 100.0f,
        .drain = 1.0f,
        .maxEnergy = 100.0f,
        .health = 100.0f,
        .maxHealth = 100.0f,
    };

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        double now = GetTime();

        // Logic
        int rng = GetRandomValue(0, 100);
        if (mobCount < 10 && rng < 5 && now - lastMob > 1.0) { // 5% chance to spawn a mob each frame, with a 1 second cooldown
            mobs[mobCount++] = (Mob){
                .position = { GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight) },
                .velocity = { 0, 0 },
                .speed = 2.0f,
                .size = 10,
                .health = 20.0f,
                .maxHealth = 50.0f,
                .damage = 10.0f,
            };
            lastMob = now;
        }

        // check for collisions with mobs
        for (size_t i = 0; i < mobCount; i++) {
            float dx = mobs[i].position.x - player.position.x;
            float dy = mobs[i].position.y - player.position.y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < mobs[i].size + player.size) {
                if (player.dashing) {
                    mobs[i].health -= 20.0f; // Damage mob when dashing
                    if (mobs[i].health <= 0) {
                        // TODO clever, but maybe do at the end
                        // Remove mob by replacing it with the last one in the array
                        mobs[i] = mobs[--mobCount];
                        i--; // Check the new mob at this index in the next iteration
                    }
                } else {
                    player.health -= mobs[i].damage;
                    if (player.health < 0) player.health = 0;
                    // simple knockback
                    player.position.x -= dx / distance * 20;
                    player.position.y -= dy / distance * 20;
                }
            }
        }

        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_D)) player.velocity.x = player.speed;
        if (IsKeyPressed(KEY_A)) player.velocity.x = -player.speed;
        if (IsKeyPressed(KEY_W)) player.velocity.y = -player.speed;
        if (IsKeyPressed(KEY_S)) player.velocity.y = player.speed;

        if (IsKeyUp(KEY_D) && IsKeyUp(KEY_A)) player.velocity.x = 0;
        if (IsKeyUp(KEY_W) && IsKeyUp(KEY_S)) player.velocity.y = 0;

        player.position.x += player.velocity.x;
        player.position.y += player.velocity.y;

        if (player.focused) {
            if (now - lastPath > 0.05 && pathI < 25) { // Update path every 0.5 seconds
                Vector2 mousePos = GetMousePosition();
                path[pathI++] = (Vector2){ mousePos.x, mousePos.y };
                lastPath = now;
            }

            player.energy -= player.drain;
            if (player.energy < 0 || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                player.energy = 0;
                player.focused = false; // Lose focus when energy depletes
                player.dashing = true;
                curI = 0;
            }
        } else if (player.dashing) {
            player.position.x = catmul_rom(
                curI > 0 ? path[curI - 1].x : player.position.x,
                path[curI].x,
                curI < pathI - 1 ? path[curI + 1].x : player.position.x,
                curI < pathI - 2 ? path[curI + 2].x : player.position.x,
                t
            );
            player.position.y = catmul_rom(
                curI > 0 ? path[curI - 1].y : player.position.y,
                path[curI].y,
                curI < pathI - 1 ? path[curI + 1].y : player.position.y,
                curI < pathI - 2 ? path[curI + 2].y : player.position.y,
                t
            );

            t += 0.2f; // Adjust this value to change the speed of movement along the path
            if (t >= 1.0f) {
                t = 0.0f;
                curI++;
                if (curI >= pathI - 2) {
                    curI = 0;
                    player.dashing = false;
                    pathI = 0; // Clear path after dashing
                }
            }
        } else {
            player.energy += player.drain * 0.5f; // Regenerate energy when not focused
            if (player.energy > player.maxEnergy) {
                player.energy = player.maxEnergy;
            }
        }

        // do this after player.focused check otherwise both will activate in the same frame
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            
            if (mousePos.x >= player.position.x - player.size && mousePos.x <= player.position.x + player.size &&
                mousePos.y >= player.position.y - player.size && mousePos.y <= player.position.y + player.size) {
                player.focused = !player.focused; // Toggle focus state
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

            // energy bar
            DrawRectangle(10, 10, 200, 20, GRAY);
            DrawRectangle(10, 10, (int)(200 * (player.energy / player.maxEnergy)), 20, BLUE);
            // health bar
            DrawRectangle(10, 40, 200, 20, GRAY);
            DrawRectangle(10, 40, (int)(200 * (player.health / player.maxHealth)), 20, RED);

            DrawCircleV(player.position, 20, player.focused ? RED : BLUE);

            if (player.focused || player.dashing) {
                for (int i = 0; i < pathI; i++) {
                    DrawCircleV(path[i], 5, BLACK);
                }
                DrawSplineCatmullRom(path, pathI, 1, BLACK); // TODO handle sample rate problems
            }

            for (size_t i = 0; i < mobCount; i++)
            {
                DrawCircleV(mobs[i].position, mobs[i].size, RED);
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
