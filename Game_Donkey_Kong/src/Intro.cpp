#include "Intro.h"

void DrawIntro(GameScreen* screen)
{
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