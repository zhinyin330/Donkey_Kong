#include "Game.h"
#include "resource_dir.h"

void RunGameLoop()
{
    // 假设你窗口已经初始化
    bool gameOver = false;

    // 加载游戏资源
    Texture player = LoadTexture("enemy.jpg");
    Texture enemy = LoadTexture("enemy.jpg");

    while (!WindowShouldClose() && !gameOver)
    {
        // 更新游戏逻辑
        // TODO: 更新玩家、敌人、场景等

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 绘制游戏元素
        DrawTexture(player, 100, 400, WHITE);
        DrawTexture(enemy, 300, 400, WHITE);

        EndDrawing();
    }

    // 卸载资源
    UnloadTexture(player);
    UnloadTexture(enemy);
}