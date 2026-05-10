#include "Menu.h"
#include "Game.h"
#include "resource_dir.h"  // 确保资源路径正确
#include "raylib.h"

void DrawMenu(GameScreen* screen)
{
    // ===== 静态资源（只加载一次）=====
    static bool initialized = false;

    static Music bgm;
    static Font font;
    static Font myFont1;// 自定义字体
    static Texture2D img1, img2, enemy1;

    static float timer = 0.0f;
    static bool blinking = true;
    static bool showFirst = true;
    static bool wasActive = false; // 控制音乐

    if (!initialized)
    {
        bgm = LoadMusicStream("audio/Title_Theme.ogg");
        SetMusicVolume(bgm, 0.5f);

        font = GetFontDefault();
        myFont1 = LoadFont("fonts/donkey-kong-nes-1981.ttf");

        img1 = LoadTexture("imagtes/imatge1.png");
        img2 = LoadTexture("imagtes/imatge2.png");

        enemy1 = LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Emote2.png");

        initialized = true;
    }

    // ===== 音乐控制=====
    if (*screen == MENU)
    {
        if (!wasActive)
        {
            PlayMusicStream(bgm);
            wasActive = true;
        }

        UpdateMusicStream(bgm);
    }
    else
    {
        if (wasActive)
        {
            StopMusicStream(bgm);
            wasActive = false;
        }
    }

    // ===== 更新 =====
    timer += GetFrameTime();

    float switchTime = 0.2f;      // 每0.2秒切换
    float blinkDuration = 2.0f;   // 闪烁总时间
    if (blinking)
    {
        showFirst = ((int)(timer / switchTime) % 2 == 0);

        if (timer >= blinkDuration)
        {
            blinking = false;
            showFirst = true;
        }
    }

    // ===== 绘制 =====
    DrawTextEx(myFont1, "1UP", { 80, 10 }, 30, 2, RED);
    DrawTextEx(myFont1, "HIGH SCORE", { 300, 10 }, 30, 2, RED);

    DrawTextEx(myFont1, "000000", { 40, 35 }, 25, 2, WHITE);
    DrawTextEx(myFont1, "000000", { 340, 35 }, 25, 2, WHITE);

    DrawTextEx(myFont1, "L=00", { 680, 80 }, 25, 2, BLUE);

    Rectangle sourceRec = { 0, 0, (float)img1.width, (float)img1.height };
    Rectangle destRec = { 100, 120, 550, 300 };

    if (showFirst)
        DrawTexturePro(img1, sourceRec, destRec, { 0,0 }, 0.0f, WHITE);
    else
        DrawTexturePro(img2, sourceRec, destRec, { 0,0 }, 0.0f, WHITE);

    // 敌人
    Rectangle sourceRec1 = { 0, 0, (float)enemy1.width, (float)enemy1.height };
    Rectangle destRec1 = { 310, 450, 170, 100 };

    DrawTexturePro(enemy1, sourceRec1, destRec1, { 0,0 }, 0.0f, WHITE);

    DrawTextEx(font, u8"© NINTENDO 1981", { 290, 620 }, 30, 1, YELLOW);

    // 检测按键
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        *screen = GAMEPLAY;   //切换
        InitGame();
       
    }

}