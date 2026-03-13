#include "Menu.h"
#include "resource_dir.h"  // 确保资源路径正确

void ShowMenuScreen()
{
    // 加载菜单资源
    
    Font font = GetFontDefault();                        // 自定义字体

    bool startGame = false;

    while (!startGame && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        // 绘制标题
        DrawTextEx(font, "Donkey Kong", { 200, 150 }, 60, 2, RED);

        // 绘制提示文字
        DrawTextEx(font, "Press ENTER or SPACE to Start", { 180, 400 }, 30, 1, DARKGRAY);

        EndDrawing();

        // 检测按键
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            startGame = true;  // 退出菜单，进入游戏
        }
    }

}