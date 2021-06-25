#include "fxlib.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"


/*printing float
void incTime() {
	    unsigned char output[20];
	    time += 0.05F;
	
	    memset(output,0,20);
	    sprintf(&output, "Time:%f",time);
	    
	    
	    locate(1,4);
	    Print((unsigned char*)output);
	    Bdisp_PutDisp_DD();
}
*/
const int screenWidth = 127;
const int screenHeight = 63;
const float gravity = 5;
//Struct Definition

struct Vector2 {
    float x;
    float y;
};
struct Player {
    int dead;
    struct Vector2 pos;
    float gravDir;
    float speed;
    int collided;
    struct Vector2 size;

};
struct Platform {
    int active;
    struct Vector2 pos;
    struct Vector2 size;
    struct Platform* next;
    struct Platform* last;
    int spawnedNext;
    int side;
};
struct Particle {
    int active;
    float lifeTime;
    struct Vector2 size;
    struct Vector2 pos;
    struct Particle* next;
    struct Particle* last;
};

//Functions

float tunnelHeight(float xPos, float freq) {
    float A = 10;
    float B = 4;
    float C = 10;
    
    float tot = A + B + C;
    
    float a = 2;
    float b = 1;
    float c = 0.1F;
    /*A = (rand() % (int)A + 1) / tot;
    B = (rand() % (int)B + 1) / tot;
    C = (rand() % (int)C + 1) / tot;*/
    A /= tot;
    B /= tot;
    C /= tot;
    
    
    
    
    return (float)(A * sin(freq * a * xPos / 1000) + B * sin(freq * b * xPos / 1000) + C * sin(freq * c * xPos / 1000));
}

/*struct Vector2 TrailSize(struct Particle trail, float timeStep) {
    float shrinkSpeed = 70;
    trail.size.x -= shrinkSpeed * timeStep;
    trail.size.y -= shrinkSpeed * timeStep;
    return trail.size;
}*/

float ClampFloat(float val, float min, float max) {
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}

float time = 0;
float d = 0;
float c = 0;


unsigned int key;
float timeStep = 1;
float scrollSpeed = -10;
float sinFreq = 10;
float sinAmp;
float tunnelWidth;
float minTunnelWidth = 20;
float narrowSpeed = 2.5F;
float globalXPos = 0;

struct Player player;
struct Platform firstPlatform;

void incTime() {
	time += 0.01F;
}

void DrawRect(struct Vector2 pos, struct Vector2 size){
    if (pos.x < 0) {
        size.x += pos.x;
        pos.x = 0;
    }
    if (pos.y < 0) {
        size.y += pos.y;
        pos.y = 0;
    }
    if (pos.x + size.x > screenWidth) size.x = screenWidth - pos.x;
    if (pos.y + size.y > screenHeight) size.y = screenWidth - pos.y;
    Bdisp_DrawLineVRAM(pos.x, pos.y, pos.x + size.x, pos.y);
    Bdisp_DrawLineVRAM(pos.x, pos.y, pos.x, pos.y + size.y);
    Bdisp_DrawLineVRAM(pos.x, pos.y + size.y, pos.x + size.x, pos.y + size.y);
    Bdisp_DrawLineVRAM(pos.x + size.x, pos.y, pos.x + size.x, pos.y + size.y);
}

void Update() {
    if (!player.dead){
        timeStep = 0.05F;
    }
    else timeStep = 0;
    
    //d = time - c;
    //    c = time;
    
    tunnelWidth -= narrowSpeed * timeStep;
    if (tunnelWidth < minTunnelWidth) {
        tunnelWidth = minTunnelWidth;
    }
    sinAmp += timeStep * 0.005f;
    sinFreq += timeStep * 0.05f;
    scrollSpeed -= timeStep * 0.05f;
    globalXPos += timeStep * -scrollSpeed;
    
    
    if (key == KEY_CTRL_DOWN) {
        if (!player.dead){
            player.gravDir = 1;
            //player.pos.y -= player.gravDir;
        }
    }
        /*else{
            player = {
                .pos = {100, screenHeight / 2},
                .gravDir = -1,
                .collided = false,
                .size = {30,30},
                .dead = false
            };
            struct Platform* platform = &firstPlatform;
            while (platform != NULL){

                
        }*/
    
    if (key == KEY_CTRL_UP) {
        if (!player.dead){
            player.gravDir = -1;
            //player.pos.y -= player.gravDir;
        }
    }
    //Gravity
    player.speed += gravity * player.gravDir * timeStep;
    player.pos.y += player.speed * timeStep;
    
    //Collisions
    player.collided = 0;
    
    
    
    {
        struct Platform* platform = &firstPlatform;
        
        c = 0;
        while (platform != NULL){
            
            int i;
            int inside;
            c++;
            platform->pos.x += timeStep * scrollSpeed;
            if (platform->active){
                for (i = 0; i < 2; i++){
                    int j;
                    for (j = -1; j < 2; j += 2){
                        struct Vector2 corner;
                        corner.x = player.pos.x + player.size.x * i;
                        corner.y = player.pos.y + player.size.y * (j + 1 )/2;
                        if (corner.x > platform->pos.x && corner.x < platform->pos.x + platform->size.x && corner.y > platform->pos.y && corner.y < platform->pos.y + platform->size.y){
                            player.collided = 1;
                            player.dead = 1;
                            KillTimer(1);
                            /*if (player.gravDir == 1) player.pos.y = platform->pos.y - player.size.y;
                            else player.pos.y = platform->pos.y + platform->size.y;
                            player.speed = 0;*/
                        }
                    }
                }
                
                /*inside = 0;
                
                for (i = 0; i < 3; i++){
                    struct Vector2 corner;
                    corner.x = player.pos.x + player.size.x;
                    corner.y = player.pos.y + player.size.y * (float)i / 2;
                    if (corner.x > platform->pos.x && corner.x < platform->pos.x + platform->size.x && corner.y > platform->pos.y && corner.y < platform->pos.y + platform->size.y){
                        inside++;
                    }  
                }
                if (inside > 1) player.dead = 1;*/
            }
            //Spawn next platform
            if (((platform->pos.x + platform->size.x) < screenWidth) && (!(platform->spawnedNext))){
                
                struct Vector2 newPos;
                struct Vector2 newSize;
                
                if (platform->side == -1){
                    newPos.x = screenWidth;
                    newPos.y = 0;
                    newSize.x = 30;
                    newSize.y = ClampFloat(tunnelHeight(globalXPos, sinFreq) * sinAmp + screenHeight / 2 + platform->side * tunnelWidth, 2, screenHeight - 2);
                }
                else{
                    newPos.x = screenWidth;
                    newPos.y = ClampFloat(tunnelHeight(globalXPos, sinFreq) * sinAmp + screenHeight / 2 + platform->side * tunnelWidth, 2, screenHeight - 2);
                    newSize.x = 30;
                    newSize.y = 1000;
                }
                d = ClampFloat(40,30,50);//ClampFloat(tunnelHeight(globalXPos, sinFreq) * sinAmp + screenHeight / 2 + platform->side * tunnelWidth, 10, screenHeight - 10);
                {
                    struct Platform instance;
                    instance.active = 1;
                    instance.pos = newPos;
                    instance.size = newSize;
                    instance.next = platform->next;
                    instance.side = platform->side; 
                    instance.last = platform;
                    instance.spawnedNext = 0;
                    c++;
                    
                    platform->next = malloc(sizeof(*platform));
                    *(platform->next) = instance;
                }
                if (platform->next->next != NULL){
                    platform->next->next->last = platform->next;
                }
                platform->spawnedNext = 1;
            }
            else if (platform->pos.x + platform->size.x < 0) {
                platform->active = 0;
                if (platform->last != NULL){
                    struct Platform* stor = platform;
                    
                    platform->last->next = platform->next;
                    if (platform->next != NULL){
                        
                        platform->next->last = platform->last;
                        
                    }
                    platform = platform->last;

                    free(stor);
                }
            }
            platform = platform->next;
        }
    }
    
    
    // Drawing
    //----------------------------------------------------------------------------------
    Bdisp_AllClr_VRAM();
    {
        
        struct Platform* platform = &firstPlatform;
        int count = 0;
        /*
        //Trail
        current = &firstParticle;
        while (current->next != NULL){
            current = current->next;
            DrawRectangleV(current->pos, current->size, LIGHTGRAY);
        }*/
        
        //Player
        DrawRect(player.pos, player.size);
        
        //Walls
        platform = &firstPlatform;
        
        
        while (platform != NULL){
            if (platform->active){
            DrawRect(platform->pos, platform->size);
            }
            platform = platform->next;
            count++;
            
        }
        
        //DrawText(TextFormat("Score: %i", -(int)globalXPos / 10 ), 10, 10, 30, LIGHTGRAY);
        //DrawText(TextFormat("FPS: %i", GetFPS()), 10, 50, 30, LIGHTGRAY);
        //DrawText(TextFormat("%i", GetFPS()), 190, 250, 20, LIGHTGRAY);
        /*for (int i = 0; i < 2; i++){
            Vector2 corner = {player.pos.x + player.size.x * i, player.pos.y + player.size.y * (player.gravDir + 1 )/2};
            DrawCircleV (corner, 5, GREEN);
        }
        
        for (int i = 0; i < 3; i++){
            Vector2 corner = {player.pos.x + player.size.x, player.pos.y + player.size.y * (float)i / 2};
            DrawCircleV (corner, 5, BLUE);
        }
        */
        //if (player.dead) DrawText("You Are Dead!", 200,200, 50, GRAY);
    }
    Bdisp_AllClr_DD();
    Bdisp_PutDisp_DD();
}

int AddIn_main(int isAppli, unsigned short OptionNum)
{
    
    //Constants
    sinAmp = screenHeight / 4;
    tunnelWidth = screenHeight / 2;
    
    
    
    //Variable Initialisation
    //player = {0,(struct Vector2){100, screenHeight / 2}, -1, 0, 0, (struct Vector2){30,30}};
    player.dead = 0;
    player.pos.x = 10;
    player.pos.y = screenHeight / 2;
    player.gravDir = -1;
    player.collided = 0;
    player.size.x = 10;
    player.size.y = 10;
    
    firstPlatform.active = 1;
    firstPlatform.pos.x = 0;
    firstPlatform.pos.y = 0;
    firstPlatform.size.x = screenWidth;
    firstPlatform.size.y = 5;
    firstPlatform.side = -1;
    firstPlatform.spawnedNext = 0;
    firstPlatform.last = NULL;
    {
        struct Platform second;
        second.active = 1;
        second.pos.x = 0;
        second.pos.y = screenHeight - 5;
        second.size.x = screenWidth;
        second.size.y = 5;
        second.side = 1;
        second.next = NULL;
        second.last = &firstPlatform;
        second.spawnedNext = 0;
        
        firstPlatform.next =  malloc(sizeof(firstPlatform));
        
        *(firstPlatform.next) = second;
    }
    

    Bdisp_AllClr_DDVRAM();
    SetTimer(1,15,Update);
    //SetTimer(1,10, incTime);
    while(1){
        GetKey(&key);
        
        
    }

    return 1;
}




//****************************************************************************
//**************                                              ****************
//**************                 Notice!                      ****************
//**************                                              ****************
//**************  Please do not change the following source.  ****************
//**************                                              ****************
//****************************************************************************


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section


#pragma section _TOP

//****************************************************************************
//  InitializeSystem
//
//  param   :   isAppli   : 1 = Application / 0 = eActivity
//              OptionNum : Option Number (only eActivity)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************
int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section

