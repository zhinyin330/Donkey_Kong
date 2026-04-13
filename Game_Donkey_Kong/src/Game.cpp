#include "Game.h"
#include "resource_dir.h"
#include "Player.h"
#include "Scene.h"
#include "Enemy.h"

void RunGameLoop() {
    bool gameOver = false;

    InitWindow(Scene::GetScreenWidth(), Scene::GetScreenHeight(), "Donkey Kong");

    InitAudioDevice();//audio
    Scene scene;
    Player player;
    Enemy enemy;
    SetTargetFPS(60);

   
   
    while (!WindowShouldClose() && !gameOver) {
        // UPDATE
        player.HandleInput();
        player.Update(scene);
        enemy.Update();

        // DRAW
        BeginDrawing();
        ClearBackground(BLACK);

        scene.Draw();
        player.Draw(); 
        enemy.Draw();
        // Dibujar Donkey Kong (opcional)
        if (dongkeyKong.id != 0) {
            DrawTextureEx(dongkeyKong, { 110, 400 }, 0.0f, 1.0f, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
}