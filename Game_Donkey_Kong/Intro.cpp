#include "Intro.h"

void DrawIntro(GameScreen* screen)
{
    ClearBackground(BLACK);

    // ===== 标题 =====
    DrawText("DONKEY KONG PROJECT", 180, 150, 30, YELLOW);

    // ===== 小组成员 =====
    DrawText("Group Members:", 280, 250, 25, WHITE);

    DrawText(" - Member 1", 300, 300, 20, LIGHTGRAY);
    DrawText(" - Member 2", 300, 330, 20, LIGHTGRAY);
    DrawText(" - Member 3", 300, 360, 20, LIGHTGRAY);

    // ===== 提示 =====
    DrawText("Press ENTER to continue", 230, 500, 20, GREEN);

    // ===== 输入 =====
    if (IsKeyPressed(KEY_ENTER))
    {
        *screen = MENU;
    }
}