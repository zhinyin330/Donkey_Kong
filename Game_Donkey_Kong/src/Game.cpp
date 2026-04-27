#include "Game.h"
#include "resource_dir.h"
#include "Player.h"
#include "Scene.h"
#include "Enemy.h"
#include "NewMechanic.h"

static Scene* gameScene = nullptr;
static Player* gamePlayer = nullptr;
static Enemy* gameEnemy = nullptr;
static bool isInitialized = false;
static NewMechanic* gameStars = nullptr;

void DrawGame(GameScreen* currentScreen)
{
    // --- 1. 延迟初始化 (只在第一次进入游戏时执行) ---
    if (!isInitialized)
    {
        // 如果你的类有构造函数，在这里 new 出来
        if (gameScene == nullptr) gameScene = new Scene();
        if (gamePlayer == nullptr) gamePlayer = new Player();
        if (gameEnemy == nullptr) gameEnemy = new Enemy();
        if (gameStars == nullptr) gameStars = new NewMechanic();

        isInitialized = true;
    }

    // --- 2. 更新逻辑 (UPDATE) ---
    float deltaTime = GetFrameTime();

    gamePlayer->HandleInput(*gameScene);
    gamePlayer->Update(*gameScene);
    gameEnemy->Update();
    gameStars->Update(deltaTime, Scene::GetScreenWidth());

    // 增加一个返回菜单的逻辑 (按下 ESC)
    if (IsKeyPressed(KEY_ESCAPE))
    {
        *currentScreen = MENU;
        // 如果希望下次进游戏重新开始，可以把 isInitialized 设为 false 并 delete 对象
        delete gameStars;
        gameStars = nullptr;
    }

    // --- 3. 绘制逻辑 (DRAW) ---
    gameScene->Draw();
    gamePlayer->Draw();
    gameEnemy->Draw();
    gameStars->Draw();


    // 如果游戏结束，切换状态
    // if (gamePlayer->IsDead()) *currentScreen = MENU;
}

// 建议在游戏完全退出时调用的清理函数
void UnloadGame()
{
    if (isInitialized)
    {
        delete gameScene;
        delete gamePlayer;
        delete gameEnemy;
        delete gameStars;
        gameScene = nullptr;
        gamePlayer = nullptr;
        gameEnemy = nullptr;
        gameStars = nullptr;
        isInitialized = false;
    }
}