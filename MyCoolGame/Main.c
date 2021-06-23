/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2016 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <raymath.h>
#include "GameHeader.h"


#define RLIGHTS_IMPLEMENTATION

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif



char errors[100][1000];
Camera3D camera = { 0 };

struct Camera3D* GetCameraRef()
{
    return &camera;
}

float textTimer;
char screenText[100];
char (*textPtr)[100];
int textLength;
void PrintToScreen(char targetText[])
{
    textLength = strlen(targetText)+1;


    //textPtr = &targetText;
    textTimer = GetTime() + 4;
    for (int i = 0; i < 100; i++)
    {
        screenText[i] = ' ';
    }

    for (int i = 0; i < textLength; i++)
    {
        screenText[i] = targetText[i];
    }

    screenText[textLength] = "\0";
}


void DrawScreenLog()
{
    /*if (IsKeyDown(KEY_FIVE))
        PrintToScreen("Picked Up 5 Pistol Ammo");*/

    if (GetTime() < textTimer)
    {
        DrawText(screenText, 420 - ((textLength*6)), 50, 20, WHITE);
    }
}

void PrepareMap()
{
    int genResult = GenerateMap();
    InitializeZombies();
    InitializeParticleSystem();



    DestroyZombies();

    //Calculate difficulty
    int zombieCount = 100 + Lerp(0, 1500, ((float)CityProgressed / 4));

    if (CityProgressed >= 5)
        zombieCount = 30;


    for (int i = 0; i < zombieCount; i++)
    {
        CreateZombie(GetEmptyTile());
    }



    GetPlayerRef()->position = (Vector3){ GetRoadStartPosition() * 2, 1.0f, -5 };
    camera.target = (Vector3){ 0.0f, 0.0f, -100.0f };


    if (CityProgressed >= 5)
        HelicopterTimer = 122;
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Tiny Apocalypse by Wolderado");

    //CityProgressed = 5;

    PrepareMap();

    // Define the camera to look into our 3d world
    //camera = { 0 };
    //camera.position = (Vector3){ 50.0f, 100.0f, 50.0f };  // Camera position
    camera.position = (Vector3){ GetRoadStartPosition() * 2, 1.0f, -5 };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, -100.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type


    SetTargetFPS(60);

    InitializePlayer(&camera);
    InitializeWeapon();
    InitializeCollectables();

    InEndScreen = true;
    GameStarted = false;




    // Main game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
            
            BeginMode3D(camera);
                Color skyColor = { .r = 0,.g = 15,.b = 55,.a = 1 };

                ClearBackground(skyColor);

                float result = ProcessInput();



                ProcessBullets();
                DrawMap(GetPlayer().position);
                ExecuteAI();
                DrawZombies();
                ProcessAndDrawParticles();
                ProcessCollectables();



            EndMode3D();

            DrawMinimap(680,20);
            DrawMinimapPlayer(676, 16);
            DrawHealthBar();
            DrawScreenLog();
            DrawMuzzle();
            DrawWeaponUI();
            ProcessAndDrawUI();
            DrawStatsUI();

            if(InEndScreen == false)
                DrawFPS(10, 10);

        EndDrawing();
    }

    //DisposeMap();

    CloseWindow();

    return 0;
}










void LightPass()
{
    RenderTexture2D screenTex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());


    Color GetPixelColor(void* srcPtr, int format);
}