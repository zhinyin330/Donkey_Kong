#include "Game.h"
#include "resource_dir.h"
#include "Player.h"
#include "Scene.h"
#include "Enemy.h"

void RunGameLoop()
{
    // 假设你窗口已经初始化
    bool gameOver = false;

    InitWindow(Scene::GetScreenWidth(), Scene::GetScreenHeight(), "Donkey Kong");
    InitAudioDevice();
    SetTargetFPS(60);
    
    InitAudioDevice();//audio
    Scene scene;
    Player player;
    Enemy enemy;

    // 加载游戏资源

    while (!WindowShouldClose() && !gameOver) {
        // UPDATE
        player.HandleInput();
        player.Update(scene);
        enemy.Update();


        // 更新游戏逻辑
        // TODO: 更新玩家、敌人、场景等

        BeginDrawing();
        ClearBackground(BLACK);

        // 绘制游戏元素
        scene.Draw();
        player.Draw(); 
        enemy.Draw();

        EndDrawing();

    }

    // 卸载资源
    CloseAudioDevice();
    CloseWindow();

}