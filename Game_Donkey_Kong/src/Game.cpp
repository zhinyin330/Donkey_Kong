#include "Game.h"
#include "resource_dir.h"

void RunGameLoop()
{
    // 假设你窗口已经初始化
    bool gameOver = false;

    // 加载游戏资源
    Texture player = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png");
    Rectangle sourceRec1 = { 0, 0, (float)player.width, (float)player.height }; // 原图大小
    Rectangle destRec1 = { 110, 300, 50, 50 };
    Vector2 origin1 = { 0, 0 };

    Texture dongkeyKong = LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_M.png");;
    Rectangle sourceRec2 = { 0, 0, (float)dongkeyKong.width, (float)dongkeyKong.height }; // 原图大小
    Rectangle destRec2 = { 110, 400, 130, 80 };
    Vector2 origin2 = { 0, 0 };
    while (!WindowShouldClose() && !gameOver)
    {
        // 更新游戏逻辑
        // TODO: 更新玩家、敌人、场景等

        BeginDrawing();
        ClearBackground(BLACK);

        // 绘制游戏元素
        DrawTexturePro(player, sourceRec1, destRec1, origin1, 0.0f, WHITE);
        DrawTexturePro(dongkeyKong, sourceRec2, destRec2, origin2, 0.0f, WHITE);


        EndDrawing();
    }

    // 卸载资源
    UnloadTexture(player);
    UnloadTexture(dongkeyKong);
}