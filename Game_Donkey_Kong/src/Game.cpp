#include "Game.h"
#include "resource_dir.h"
#include "Player.h"
#include "Scene.h"
#include "Scene2.h"
#include "Enemy.h"
#include "NewMechanic.h"
#include "GameScene.h"

static GameScene* gameScene = nullptr;
static Player* gamePlayer = nullptr;
static Enemy* gameEnemy = nullptr;
static NewMechanic* gameStars = nullptr;
static bool isInitialized = false;
static bool shouldReset = false;
static bool isScene2 = false;  // Para saber qué escena está activa

void InitGame()
{
    // Limpiar antes de inicializar
    CleanupGame();

    gameScene = new Scene();
    gamePlayer = new Player();
    gameEnemy = new Enemy();
    gameEnemy->SetBehavior(EnemyBehavior::THROW_BARRELS);  // Scene 1: lanza barriles
    gameStars = new NewMechanic();

    isScene2 = false;
    isInitialized = true;
    shouldReset = false;
}

void InitGameScene2()
{
    CleanupGame();

    gameScene = new Scene2();
    gamePlayer = new Player();
    gameEnemy = new Enemy();
    gameEnemy->SetBehavior(EnemyBehavior::STATIONARY);     // Scene 2: decorativo
    gameEnemy->SetPosition(317, 130);                      // Posición diferente
    gameStars = new NewMechanic();

    isScene2 = true;
    isInitialized = true;
    shouldReset = false;
}

void CleanupGame()
{
    if (isInitialized)
    {

        if (gameScene != nullptr)
        {
            delete gameScene;
            gameScene = nullptr;
        }

        if (gamePlayer != nullptr)
        {
            delete gamePlayer;
            gamePlayer = nullptr;
        }

        if (gameEnemy != nullptr)
        {
            delete gameEnemy;
            gameEnemy = nullptr;
        }

        if (gameStars != nullptr)
        {
            delete gameStars;
            gameStars = nullptr;
        }

        isInitialized = false;
        isScene2 = false;
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
        if (gameStars == nullptr) gameStars = new NewMechanic();

        isScene2 = false;
        isInitialized = true;
    }

    // --- 2. 更新逻辑 (UPDATE) ---
    float deltaTime = GetFrameTime();

    gamePlayer->HandleInput(*gameScene);
    gamePlayer->Update(*gameScene);
    gameEnemy->Update(*gameScene);
    gameStars->Update(deltaTime, GameScene::GetScreenWidth());
    gameStars->CheckCollisionWithPlayer(gamePlayer);
    // ========== 新增 ==========
    if (isScene2) {
        Scene2* scene2 = dynamic_cast<Scene2*>(gameScene);
        if (scene2) {
            Rectangle playerHitbox = {
                gamePlayer->GetPosition().x,
                gamePlayer->GetPosition().y + 2 * gamePlayer->GetScale(),
                gamePlayer->GetTextureWidth() * gamePlayer->GetScale(),
                14 * gamePlayer->GetScale()
            };
            scene2->CheckItemCollision(playerHitbox, gamePlayer);
        }
    }
    // ========== 新增结束 ==========
    
    // Temporal cambio de escenas
    if (IsKeyPressed(KEY_T) && !isScene2)
    {
        TraceLog(LOG_INFO, "Transición forzada a Scene2");
        InitGameScene2();
        return;
    }

    if (!isScene2)
    {
        Vector2 playerPos = gamePlayer->GetPosition();
        float playerWidth = gamePlayer->GetTextureWidth() * gamePlayer->GetScale();
        float playerHeight = gamePlayer->GetTextureHeight() * gamePlayer->GetScale();

        Rectangle playerRect = {
            playerPos.x,
            playerPos.y,
            playerWidth,
            playerHeight
        };

        Rectangle transitionZone = gameScene->GetTransitionZone();

        // Verificar colisión con zona de transición
        if (CheckCollisionRecs(playerRect, transitionZone))
        {
            gameScene->SetTransitionReached(true);
        }

        // Si ya se alcanzó la transición, cambiar a Scene2
        if (gameScene->IsTransitionReached())
        {
            TraceLog(LOG_INFO, "¡Cambiando a Scene2!");
            InitGameScene2();
            return;  // Salir para evitar dibujar con escena antigua
        }
    }
    // hasta aqui
    
    
    // 增加一个返回菜单的逻辑 (按下 ESC)
    if (IsKeyPressed(KEY_ESCAPE))
    {
        *currentScreen = MENU;
        // 如果希望下次进游戏重新开始，可以把 isInitialized 设为 false 并 delete 对象
        CleanupGame();
        return;
    }

    // --- 3. 绘制逻辑 (DRAW) ---
    gameScene->Draw();
    gamePlayer->Draw();
    gameEnemy->Draw();
    gameStars->Draw();

    // temporal: mostrar info
    if (isScene2)
    {
        DrawText("SCENE 2 - Nivel 2", 10, 10, 20, GREEN);
    }
    else
    {
        DrawText("SCENE 1 - Nivel 1", 10, 10, 20, YELLOW);
    }

    // ===== RESET =====
    if (shouldReset)
    {
        CleanupGame();
        shouldReset = false;
    }
}

void UnloadGame()
{
    CleanupGame();
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

// Función auxiliar para cambiar de escena manualmente
void SwitchToScene2()
{
    InitGameScene2();
}

// Función para reiniciar el juego
void ResetGame()
{
    shouldReset = true;
}