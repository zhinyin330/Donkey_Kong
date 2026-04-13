#include "Game.h"
#include "resource_dir.h"
#include "Player.h"
#include "Scene.h"

void RunGameLoop() {
    bool gameOver = false;

    InitWindow(Scene::GetScreenWidth(), Scene::GetScreenHeight(), "Donkey Kong");
    SetTargetFPS(60);

    Scene scene;
    Player player;

    // Textura de Donkey Kong (opcional)
    Texture dongkeyKong = LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_M.png");

    while (!WindowShouldClose() && !gameOver) {
        // UPDATE
        player.HandleInput();
        player.Update(scene);

        // DRAW
        BeginDrawing();
        ClearBackground(BLACK);

        scene.Draw();
        player.Draw();

        // Dibujar Donkey Kong (opcional)
        if (dongkeyKong.id != 0) {
            DrawTextureEx(dongkeyKong, { 110, 400 }, 0.0f, 1.0f, WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(dongkeyKong);
    CloseWindow();
}