#include "Intro.h"

// 全局静态变量（放在函数外面）
static Music startupMusic;
static bool musicPlayed = false;

void DrawIntro(GameScreen* screen)
{
    ClearBackground(BLACK);
    // 第一次进入时加载并播放音乐
    if (!musicPlayed)
    {
        startupMusic = LoadMusicStream("audio/Startup.mp3");
        SetMusicVolume(startupMusic, 0.8f);
        PlayMusicStream(startupMusic);
        musicPlayed = true;
    }

    // 更新音乐流（让音乐播放）
    UpdateMusicStream(startupMusic);


    ClearBackground(BLACK);


    ClearBackground(BLACK);
    // ===== 标题 =====
    DrawText("DONKEY KONG PROJECT", 200, 150, 30, YELLOW);

    // ===== 小组成员 =====
    DrawText("Group Untitled:", 300, 280, 25, WHITE);

    DrawText(" Daniel Castillero", 300, 330, 20, LIGHTGRAY);
    DrawText(" Xueheng Wang", 305, 360, 20, LIGHTGRAY);
    DrawText(" Diyang Zhou", 315, 390, 20, LIGHTGRAY);
    DrawText(" Zhiyi Zhou", 320, 420, 20, LIGHTGRAY);

    // ===== 提示 =====
    DrawText("Press ENTER to continue", 260, 500, 20, GREEN);

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
}