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
<<<<<<< HEAD
    
    Scene scene;
    Player player;
    SetTargetFPS(60);

    // 加载游戏资源

    Texture dongkeyKong = LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_M.png");;
    Rectangle sourceRec2 = { 0, 0, (float)dongkeyKong.width, (float)dongkeyKong.height }; // 原图大小
    Rectangle destRec2 = { 110, 400, 130, 80 };
    Vector2 origin2 = { 0, 0 };
    while (!WindowShouldClose() && !gameOver)
    {
        

=======

    InitAudioDevice();//audio
    Scene scene;
    Player player;
    Enemy enemy;
    SetTargetFPS(60);

   
   
    while (!WindowShouldClose() && !gameOver) {
>>>>>>> 73a6ffbaf5165031e9411a3bdf07db67fd4e242b
        // UPDATE
        player.HandleInput();
        player.Update(scene);
        enemy.Update();


        // 更新游戏逻辑
        // TODO: 更新玩家、敌人、场景等

        BeginDrawing();
        ClearBackground(BLACK);

        scene.Draw();
        player.Draw(); 
<<<<<<< HEAD

        // 绘制游戏元素
        DrawTexturePro(dongkeyKong, sourceRec2, destRec2, origin2, 0.0f, WHITE);

=======
        enemy.Draw();
        // Dibujar Donkey Kong (opcional)
        if (dongkeyKong.id != 0) {
            DrawTextureEx(dongkeyKong, { 110, 400 }, 0.0f, 1.0f, WHITE);
        }
>>>>>>> 73a6ffbaf5165031e9411a3bdf07db67fd4e242b

        EndDrawing();

    }

<<<<<<< HEAD
    // 卸载资源
    UnloadTexture(dongkeyKong);
=======
    CloseWindow();
>>>>>>> 73a6ffbaf5165031e9411a3bdf07db67fd4e242b
}