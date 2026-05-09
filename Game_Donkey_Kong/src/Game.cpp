#include "Game.h"
#include "resource_dir.h"
#include "Player.h"
#include "Scene.h"
#include "Scene2.h"
#include "Enemy.h"
#include "NewMechanic.h"
#include "GameScene.h"
#include "Transition.h"

static GameScene* gameScene = nullptr;
static Player* gamePlayer = nullptr;
static Enemy* gameEnemy = nullptr;
static NewMechanic* gameStars = nullptr;
static bool isInitialized = false;
static bool shouldReset = false;
static bool isScene2 = false;  // Para saber qué escena está activa
static Transition* gameTransition = nullptr;


void InitGame()
{
    // Limpiar antes de inicializar
    CleanupGame();

    gameScene = new Scene();
    gamePlayer = new Player();
    gameEnemy = new Enemy();
    gameEnemy->SetBehavior(EnemyBehavior::THROW_BARRELS);  // Scene 1: lanza barriles
    gameStars = new NewMechanic();
    gameTransition = new Transition();

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
    gameEnemy->SetPosition(325, 130);                      // Posición diferente
    gameStars = new NewMechanic();

    isScene2 = true;
    isInitialized = true;
    shouldReset = false;
    gameTransition = new Transition();
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

        if (gameTransition != nullptr) {
            delete gameTransition;
            gameTransition = nullptr;
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
        if (gameTransition == nullptr) gameTransition = new Transition();

        isScene2 = false;
        isInitialized = true;
    }

    // --- 2. 更新逻辑 (UPDATE) ---
    float deltaTime = GetFrameTime();

// 更新场景2的音乐
if (isScene2 && gameScene != nullptr) {
    Scene2* scene2 = dynamic_cast<Scene2*>(gameScene);
    if (scene2) {
        scene2->UpdateMusic();
    }
}
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

    // ==========================================
   // TRANSICIÓN A SCENE2
   // ==========================================
    if (!isScene2 && gameScene->IsTransitionReached()) {
        if (gameTransition != nullptr && !gameTransition->IsFinished()) {
            gameTransition->Update();

            // Dibujar la escena detenida de fondo
            gameScene->Draw();
            gamePlayer->Draw();
            gameEnemy->Draw();

            // Dibujar la transición encima
            gameTransition->Draw();

            if (gameTransition->IsFinished()) {
                InitGameScene2();
            }
            return;
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

    if (!isScene2) {
        Vector2 princessPos = gameScene->GetPrincessPosition();
        float princessScale = gameScene->GetPrincessScale();

        if (princessScale > 0) {  // Solo si la escena tiene princesa
            Rectangle princessRect = {
                princessPos.x,
                princessPos.y,
                gamePlayer->GetTextureWidth() * princessScale,   // Usamos tamaño aprox
                gamePlayer->GetTextureHeight() * princessScale
            };

            Rectangle playerRect = {
                gamePlayer->GetPosition().x,
                gamePlayer->GetPosition().y,
                gamePlayer->GetTextureWidth() * gamePlayer->GetScale(),
                gamePlayer->GetTextureHeight() * gamePlayer->GetScale()
            };

            if (CheckCollisionRecs(playerRect, princessRect)) {
                TraceLog(LOG_INFO, "¡Princesa rescatada! Iniciando transicion");
                gameScene->SetTransitionReached(true);  
                if (gameTransition != nullptr) {
                    gameTransition->Start(2, gamePlayer->GetStarCount() * 100, gamePlayer->GetStarCount());
                }
                return;
            }
        }
    }

    
    
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

    // ========== 显示得分（左上角）==========
    if (gamePlayer != nullptr) {
        int score = gamePlayer->GetScore();
        char scoreText[32];
        sprintf(scoreText, "SCORE: %04d", score);

        // 绘制得分背景（半透明黑色）
        DrawRectangle(10, 10, 120, 30, Fade(BLACK, 0.7f));
        // 绘制得分文字（白色）
        DrawText(scoreText, 15, 15, 20, WHITE);
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