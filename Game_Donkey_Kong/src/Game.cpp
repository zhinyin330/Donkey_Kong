#include "Game.h"
#include "resource_dir.h"
#include "Player.h"
#include "Scene.h"
#include "Enemy.h"

static Scene* gameScene = nullptr;
static Player* gamePlayer = nullptr;
static Enemy* gameEnemy = nullptr;
static bool isInitialized = false;
static bool shouldReset = false;

void InitGame()
{
    gameScene = new Scene();
    gamePlayer = new Player();
    gameEnemy = new Enemy();

    isInitialized = true;
    shouldReset = false;
}
void CleanupGame()
{
    if (isInitialized)
    {
        delete gameScene;
        delete gamePlayer;
        delete gameEnemy;
        gameScene = nullptr;
        gamePlayer = nullptr;
        gameEnemy = nullptr;
        isInitialized = false;
    }
}

void DrawGame(GameScreen* currentScreen)
{
    // --- 1. 延迟初始化 (只在第一次进入游戏时执行) ---
    if (!isInitialized)
    {
        // 如果你的类有构造函数，在这里 new 出来
        if (gameScene == nullptr) gameScene = new Scene();
        if (gamePlayer == nullptr) gamePlayer = new Player();
        if (gameEnemy == nullptr) gameEnemy = new Enemy();

        isInitialized = true;
    }

    // --- 2. 更新逻辑 (UPDATE) ---
    gamePlayer->HandleInput(*gameScene);
    gamePlayer->Update(*gameScene);
    gameEnemy->Update(*gameScene);

    // 增加一个返回菜单的逻辑 (按下 ESC)
    if (IsKeyPressed(KEY_ESCAPE))
    {
        *currentScreen = MENU;
        // 如果希望下次进游戏重新开始，可以把 isInitialized 设为 false 并 delete 对象
    }

    // --- 3. 绘制逻辑 (DRAW) ---
    gameScene->Draw();
    gamePlayer->Draw();
    gameEnemy->Draw();

    // ===== RESET =====
    if (shouldReset)
    {
        CleanupGame();
    }
}


void UnloadGame()
{
    CleanupGame();
}