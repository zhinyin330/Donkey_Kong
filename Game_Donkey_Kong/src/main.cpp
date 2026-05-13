/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "Menu.h"
#include "Game.h"
#include "Intro.h"

int main()
{
    // Tell the window to use vsync and work on high DPI displays
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

    // Create the window and OpenGL context
    InitWindow(800, 704, "Donkey Kong");
    SetTargetFPS(60);

    // Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
    //帮程序找到资源文件夹
    SearchAndSetResourceDir("resources");
    // 当前界面
    GameScreen currentScreen = INTRO;
    InitAudioDevice();//音乐

    // 游戏主循环
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        // 根据状态绘制不同界面
        switch (currentScreen)
        {
            case INTRO:
                DrawIntro(&currentScreen);
                break;

            case MENU:
                DrawMenu(&currentScreen);
                break;

            case GAMEPLAY:
            case TRANSITION:
                DrawGame(&currentScreen);
                break;

            case GAME_OVER:
                DrawGameOver(&currentScreen);
                break;

            default:
                break;
        }

        EndDrawing();
    }

    UnloadGame();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
