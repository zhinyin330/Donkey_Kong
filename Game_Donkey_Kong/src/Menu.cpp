#include "Menu.h"
#include "resource_dir.h"  // 确保资源路径正确

void ShowMenuScreen()
{
    // 加载菜单资源
    
    Font font = GetFontDefault();                        // 自定义字体
    Font myFont1 = LoadFont("fonts/donkey-kong-nes-1981.ttf");

    bool startGame = false;
    //图片闪烁控制变量
    float timer = 0.0f;
    Texture2D img1 = LoadTexture("imagtes/imatge1.png");
    Texture2D img2 = LoadTexture("imagtes/imatge2.png");
    float blinkDuration = 2.0f;   // 闪烁总时间
    float switchTime = 0.2f;      // 每0.2秒切换
    bool blinking = true;
    bool showFirst = true;

    // 设置图片显示大小和位置
    Rectangle sourceRec = { 0, 0, (float)img1.width, (float)img1.height }; // 原图大小
    Rectangle destRec = { 140, 180, 500, 250 };                             // 显示位置和大小
    Vector2 origin = { 0, 0 };
    //敌人
    Texture2D enemy1 = LoadTexture("Enemy/enemy1.jpg");

    while (!startGame && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        timer += GetFrameTime();

        DrawTextEx(myFont1, "1UP", { 80, 10 }, 30, 2, RED);
        DrawTextEx(myFont1, "HIGH SCORE", { 300, 10 }, 30, 2, RED);

        DrawTextEx(myFont1, "000000", { 40, 35 }, 25, 2, WHITE);
        DrawTextEx(myFont1, "000000", { 340, 35 }, 25, 2, WHITE);

        DrawTextEx(myFont1, "L=00", { 680, 80 }, 25, 2, BLUE);

        // 标题图片闪烁
        if (blinking)
        {
            if ((int)(timer / switchTime) % 2 == 0)
                showFirst = true;
            else
                showFirst = false;

            if (timer >= blinkDuration)
            {
                blinking = false;
                showFirst = true;
            }
        }

        if (showFirst)
            DrawTexturePro(img1, sourceRec, destRec, origin, 0.0f, WHITE);
        else
            DrawTexturePro(img2, sourceRec, destRec, origin, 0.0f, WHITE);
        // Mario
        DrawRectangle(100, 470, 30, 30, BLUE);

        // 绘制提示文字
        DrawTextEx(font, u8"© NINTENDO 1981", { 300, 720 }, 30, 1, YELLOW);

        EndDrawing();

        // 检测按键
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        {
            startGame = true;  // 退出菜单，进入游戏
        }
    }

}