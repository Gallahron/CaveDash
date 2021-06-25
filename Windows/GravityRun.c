#include "raylib.h"
#include "stdlib.h"
#include "math.h"

float tunnelHeight(float xPos, float freq) {
    float A = 10;
    float B = 4;
    float C = 10;
    
    float tot = A + B + C;
    /*A = (rand() % (int)A + 1) / tot;
    B = (rand() % (int)B + 1) / tot;
    C = (rand() % (int)C + 1) / tot;*/
    A /= tot;
    B /= tot;
    C /= tot;
    
    float a = 2;
    float b = 1;
    float c = 0.1F;
    
    
    return (float)(A * sin(freq * a * xPos / 1000) + B * sin(freq * b * xPos / 1000) + C * sin(freq * c * xPos / 10));
}
Vector2 TrailSize(Vector2 size, float timeStep) {
    
}

float ClampFloat(float val, float min, float max) {
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    const float gravity = 400;

    InitWindow(screenWidth, screenHeight, "Gravity Run");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    
    struct Player {
        bool dead;
        Vector2 pos;
        float gravDir;
        float speed;
        bool collided;
        Vector2 size;
        Color color;
    };
    struct Platform {
        Vector2 pos;
        Vector2 size;
        Color color;
        struct Platform* next;
        struct Platform* last;
        bool spawnedNext;
        int side;
    };
    
    struct Player player = {
        .pos = {100, screenHeight / 2},
        .gravDir = -1,
        .collided = false,
        .size = {30,30},
        .color = RED,
        .dead = false
    };
    /*struct Platform platforms[] = {
        {.pos = {0, screenHeight - 50}, .size = {screenWidth, 50}, .color = BLUE},
        {.pos = {0, 0}, .size = {screenWidth, 50}, .color = RED},
    };*/
    
    struct Platform firstPlatform = {.pos = {0, -40}, .size = {screenWidth, 50}, .color = RED, .side = -1};
    {
        firstPlatform.next = malloc(sizeof(firstPlatform));
        struct Platform second = {.pos = {0, screenHeight - 10}, .size = {screenWidth, 50}, .color = BLUE, .side = 1, .last = &firstPlatform};
        *(firstPlatform.next) = second;
    }
   
    
    
    float timeStep = 1;
    float scrollSpeed = -200;
    float sinFreq = 1;
    float sinAmp = screenHeight / 4;
    float tunnelWidth = screenHeight / 2 + 40;
    float minTunnelWidth = 100;
    float narrowSpeed = 2.5F;
    float globalXPos = 0;
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (!player.dead){
            timeStep = GetFrameTime();
        }
        else timeStep = 0;
        
        tunnelWidth -= narrowSpeed * timeStep;
        if (tunnelWidth < minTunnelWidth) {
            tunnelWidth = minTunnelWidth;
        }
        sinAmp += timeStep * 0.005f;
        sinFreq += timeStep * 0.005f;

        if (IsKeyPressed(KEY_SPACE)) {
            if (!player.dead){
                player.gravDir *= -1;
                player.pos.y -= player.gravDir;
            }
            /*else{
                player = {
                    .pos = {100, screenHeight / 2},
                    .gravDir = -1,
                    .collided = false,
                    .size = {30,30},
                    .color = RED,
                    .dead = false
                };
                struct Platform* platform = &firstPlatform;
                while (platform != NULL){
                    
            }*/
        }
        
        
        //Gravity
        player.speed += gravity * player.gravDir * timeStep;
        player.pos.y += player.speed * timeStep;
        
        //Collisions
        player.collided = false;
        
        globalXPos += timeStep * scrollSpeed;
        
        struct Platform* platform = &firstPlatform;
        while (platform != NULL){
            platform->pos.x += timeStep * scrollSpeed;
            
            for (int i = 0; i < 2; i++){
                for (int j = -1; j < 2; j += 2){
                    Vector2 corner = {player.pos.x + player.size.x * i, player.pos.y + player.size.y * (j + 1 )/2};
                    if (corner.x > platform->pos.x && corner.x < platform->pos.x + platform->size.x && corner.y > platform->pos.y && corner.y < platform->pos.y + platform->size.y){
                        player.collided = true;
                        player.dead = true;
                        /*if (player.gravDir == 1) player.pos.y = platform->pos.y - player.size.y;
                        else player.pos.y = platform->pos.y + platform->size.y;
                        player.speed = 0;*/
                    }
                }
            }
            
            int inside = 0;
            for (int i = 0; i < 3; i++){
                Vector2 corner = {player.pos.x + player.size.x, player.pos.y + player.size.y * (float)i / 2};
                if (corner.x > platform->pos.x && corner.x < platform->pos.x + platform->size.x && corner.y > platform->pos.y && corner.y < platform->pos.y + platform->size.y){
                    inside++;
                }  
            }
            if (inside > 1) player.dead = true;
            
            //Spawn next platform
            if (platform->pos.x + platform->size.x < screenWidth && !platform->spawnedNext){
                struct Platform instance = {.pos = {screenWidth, ClampFloat(tunnelHeight(globalXPos, sinFreq) * sinAmp + screenHeight / 2 + platform->side * tunnelWidth, -platform->size.y + 10, screenHeight - 10)}, .size = {15, 50}, .color = BLUE, .next = platform->next, .side = platform->side, .last = platform};
                
                platform->next = malloc(sizeof(*platform));
                *(platform->next) = instance;
                if (platform->next->next != NULL){
                    platform->next->next->last = platform->next;
                }
                platform->spawnedNext = true;
            }
            else if (platform->pos.x + platform->size.x < 0) {
                if (platform->last != NULL){
                    platform->last->next = platform->next;
                    platform->next->last = platform->last;
                }
            }
            
            platform = platform->next;
        }
        
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            DrawRectangleV(player.pos, player.size, player.color);
            

            platform = &firstPlatform;
            int count = 0;
            
            while (platform != NULL){
                DrawRectangleV(platform->pos, platform->size, platform->color);
                platform = platform->next;
                count++;
            }
            
            DrawText(TextFormat("Score: %i", -(int)globalXPos / 10 ), 10, 10, 30, LIGHTGRAY);
            //DrawText(TextFormat("%i", GetFPS()), 190, 250, 20, LIGHTGRAY);
            for (int i = 0; i < 2; i++){
                Vector2 corner = {player.pos.x + player.size.x * i, player.pos.y + player.size.y * (player.gravDir + 1 )/2};
                DrawCircleV (corner, 5, GREEN);
            }
            /*
            for (int i = 0; i < 3; i++){
                Vector2 corner = {player.pos.x + player.size.x, player.pos.y + player.size.y * (float)i / 2};
                DrawCircleV (corner, 5, BLUE);
            }*/
            if (player.dead) DrawText("You Are Dead!", 200,200, 50, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}