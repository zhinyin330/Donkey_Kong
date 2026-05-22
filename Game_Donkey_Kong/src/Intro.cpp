#include "Intro.h"

// 全局静态变量（放在函数外面）
static Music startupMusic;
static bool musicPlayed = false;
static Texture2D introImage;
static bool imageLoaded = false;

void DrawIntro(GameScreen* screen)
{
    ClearBackground(BLACK);
    // 第一次进入时加载并播放音乐
    if (!musicPlayed)
    {
        startupMusic = LoadMusicStream("audio/Start.mp3");
        SetMusicVolume(startupMusic, 0.8f);
        PlayMusicStream(startupMusic);
        musicPlayed = true;
    }

    if (!imageLoaded)
    {
        introImage = LoadTexture("UI/Logo_UPC.png");
        imageLoaded = true;
    }

    // 更新音乐流（让音乐播放）
    UpdateMusicStream(startupMusic);


    ClearBackground(BLACK);


    ClearBackground(BLACK);
    // ===== 标题 =====
    DrawText("DONKEY KONG PROJECT", 200, 150, 35, YELLOW);

    // ===== 小组成员 =====
    DrawText("Group Untitled:", 300, 280, 25, WHITE);

    DrawText(" Daniel Castillero", 310, 330, 20, LIGHTGRAY);
    DrawText(" Xueheng Wang", 325, 360, 20, LIGHTGRAY);
    DrawText(" Diyang Zhou", 330, 390, 20, LIGHTGRAY);
    DrawText(" Zhiyi Zhou", 340, 420, 20, LIGHTGRAY);

    // ===== 图片设置 =====

    // 缩放大小（1.0 原大小）
    float scale = 0.04f;

    // 计算图片显示后的宽高
    float imageWidth = introImage.width * scale;
    float imageHeight = introImage.height * scale;

    // 居中位置
    float imageX = (GetScreenWidth() - imageWidth) / 2;

    // Y位置
    float imageY = 500;

    // 绘制图片
    DrawTextureEx(introImage,Vector2{ imageX, imageY },0.0f, scale,WHITE);



    // ===== 提示 =====
    DrawText("Press ENTER to continue", 260, 600, 20, GREEN);

    // ===== 输入 =====
    if (IsKeyPressed(KEY_ENTER))
    {
        *screen = MENU;
    }
}
void UnloadIntroMusic()
{
    static Music startupMusic;
    UnloadMusicStream(startupMusic);
    if (imageLoaded)
    {
        UnloadTexture(introImage);
    }
}