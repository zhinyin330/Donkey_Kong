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
static bool isScene2 = false;
static Transition* gameTransition = nullptr;
static int currentLevel = 1;
static int totalScore = 0;
static int totalStars = 0;
// 锤子变量 - 显式初始化
static Texture2D hammerTexture = { 0 };
static Vector2 hammerPosition = { 400, 300 };
static bool hammerActive = true;
static bool hammerCollected = false;
static float hammerScale = 2.3f;


void InitGame()
{
    CleanupGame();

    gameScene = new Scene();
    gamePlayer = new Player();
    gameEnemy = new Enemy();
    gameEnemy->SetBehavior(EnemyBehavior::THROW_BARRELS);
    gameStars = new NewMechanic();
    gameTransition = new Transition();

    for (int i = 0; i < totalStars; i++) {
        gamePlayer->AddStar();
    }

    isScene2 = false;
    isInitialized = true;
    shouldReset = false;

    hammerTexture = LoadTexture("items/Dk_Hammer_Up.png");
    hammerActive = true;
    hammerCollected = false;
    hammerPosition = { 600.0f, 540.0f };
}

void InitGameScene2()
{
    CleanupGame();

    gameScene = new Scene2();
    gamePlayer = new Player();
    gameEnemy = new Enemy();
    gameEnemy->SetBehavior(EnemyBehavior::DECORATIVE_CYCLE);
    gameEnemy->SetPosition(340, 130);
    gameStars = new NewMechanic();

    for (int i = 0; i < totalStars; i++) {
        gamePlayer->AddStar();
    }

    isScene2 = true;
    isInitialized = true;
    shouldReset = false;
    gameTransition = new Transition();

    hammerTexture = LoadTexture("items/Dk_Hammer_Up.png");
    hammerActive = true;
    hammerCollected = false;
    hammerPosition = { 400.0f, 300.0f };
}

void CleanupGame()
{
    if (isInitialized)
    {
        if (gameScene != nullptr) { delete gameScene; gameScene = nullptr; }
        if (gamePlayer != nullptr) { delete gamePlayer; gamePlayer = nullptr; }
        if (gameEnemy != nullptr) { delete gameEnemy; gameEnemy = nullptr; }
        if (gameStars != nullptr) { delete gameStars; gameStars = nullptr; }
        if (gameTransition != nullptr) { delete gameTransition; gameTransition = nullptr; }

        isInitialized = false;
        isScene2 = false;
    }
}


void DrawGame(GameScreen* currentScreen)
{
    // --- 1. Inicialización ---
    if (!isInitialized)
    {
        if (gameScene == nullptr) gameScene = new Scene();
        if (gamePlayer == nullptr) gamePlayer = new Player();
        if (gameEnemy == nullptr) gameEnemy = new Enemy();
        if (gameStars == nullptr) gameStars = new NewMechanic();
        if (gameTransition == nullptr) gameTransition = new Transition();

        isScene2 = false;
        isInitialized = true;
    }

    // --- 2. UPDATE ---
    float deltaTime = GetFrameTime();

    if (isScene2) {
        Scene2* scene2 = dynamic_cast<Scene2*>(gameScene);
        if (scene2) scene2->UpdateMusic();
    }

    // ========== CAÍDA DE DK (Scene2) ==========
    if (isScene2) {
        Scene2* scene2 = dynamic_cast<Scene2*>(gameScene);
        if (scene2) {
            scene2->UpdateDkFall(deltaTime);

            if (scene2->IsDkFalling() && !scene2->IsDkLanded()) {
                // Pausa: DK cayendo
                gameScene->Draw();
                gamePlayer->Draw();
                gameStars->Draw();
                scene2->DrawDkFalling();
                return;
            }

            if (scene2->IsDkOnPlatform()) {
                scene2->UpdateDkBounce(deltaTime);
            }
        }
    }

    gamePlayer->HandleInput(*gameScene);
    gamePlayer->Update(*gameScene);
    gameEnemy->Update(*gameScene);
    gameStars->Update(deltaTime, GameScene::GetScreenWidth());
    gameStars->CheckCollisionWithPlayer(gamePlayer);

    

    // Martillo
    if (hammerActive && !hammerCollected && gamePlayer != nullptr) {
        Rectangle hammerHitbox = {
            hammerPosition.x,
            hammerPosition.y,
            hammerTexture.width * hammerScale,
            hammerTexture.height * hammerScale
        };
        Rectangle playerHitbox = gamePlayer->GetHitbox();
        if (CheckCollisionRecs(playerHitbox, hammerHitbox)) {
            hammerCollected = true;
            hammerActive = false;
            gamePlayer->SetHasHammer(true);
        }
    }

    // Colisiones Scene2 (items + botones)
    if (isScene2) {
        Scene2* scene2 = dynamic_cast<Scene2*>(gameScene);
        if (scene2) {
            Rectangle feetHitbox = {
                gamePlayer->GetPosition().x,
                gamePlayer->GetPosition().y + 2 * gamePlayer->GetScale(),
                gamePlayer->GetTextureWidth() * gamePlayer->GetScale(),
                14 * gamePlayer->GetScale()
            };
            scene2->CheckItemCollision(feetHitbox, gamePlayer);

            Rectangle fullHitbox = {
                gamePlayer->GetPosition().x,
                gamePlayer->GetPosition().y,
                gamePlayer->GetTextureWidth() * gamePlayer->GetScale(),
                gamePlayer->GetTextureHeight() * gamePlayer->GetScale()
            };
            scene2->CheckButtonCollision(fullHitbox);
        }
    }

    // --- 3. TRANSICIÓN ---
    if (gameScene->IsTransitionReached()) {
        if (gameTransition != nullptr && !gameTransition->IsFinished()) {
            gameTransition->Update();
            gameScene->Draw();
            gamePlayer->Draw();
            gameEnemy->Draw();
            gameTransition->Draw();

            if (gameTransition->IsFinished()) {
                if (!isScene2) InitGameScene2();
                else InitGame();
            }
            return;
        }
    }

    // --- 4. Tecla T ---
    if (IsKeyPressed(KEY_T) && !isScene2) {
        InitGameScene2();
        return;
    }

    // --- 5. Princesa Scene 1 ---
    if (!isScene2) {
        Vector2 princessPos = gameScene->GetPrincessPosition();
        float princessScale = gameScene->GetPrincessScale();

        if (princessScale > 0) {
            Rectangle princessRect = {
                princessPos.x, princessPos.y,
                gamePlayer->GetTextureWidth() * princessScale,
                gamePlayer->GetTextureHeight() * princessScale
            };
            Rectangle playerRect = {
                gamePlayer->GetPosition().x, gamePlayer->GetPosition().y,
                gamePlayer->GetTextureWidth() * gamePlayer->GetScale(),
                gamePlayer->GetTextureHeight() * gamePlayer->GetScale()
            };
            if (CheckCollisionRecs(playerRect, princessRect)) {
                totalStars = gamePlayer->GetStarCount();
                totalScore += gamePlayer->GetStarCount() * 100;
                gameScene->SetTransitionReached(true);
                if (gameTransition != nullptr) {
                    gameTransition->Start(currentLevel, totalScore, totalStars);
                }
                return;
            }
        }
    }

    // --- 6. Princesa Scene 2 ---
    if (isScene2) {
        Vector2 princessPos = gameScene->GetPrincessPosition();
        float princessScale = gameScene->GetPrincessScale();

        if (princessScale > 0) {
            Rectangle princessRect = {
                princessPos.x, princessPos.y,
                gamePlayer->GetTextureWidth() * princessScale,
                gamePlayer->GetTextureHeight() * princessScale
            };
            Rectangle playerRect = {
                gamePlayer->GetPosition().x, gamePlayer->GetPosition().y,
                gamePlayer->GetTextureWidth() * gamePlayer->GetScale(),
                gamePlayer->GetTextureHeight() * gamePlayer->GetScale()
            };
            if (CheckCollisionRecs(playerRect, princessRect)) {
                totalStars = gamePlayer->GetStarCount();
                totalScore += gamePlayer->GetStarCount() * 100;
                currentLevel++;
                gameScene->SetTransitionReached(true);
                if (gameTransition != nullptr) {
                    gameTransition->Start(currentLevel, totalScore, totalStars);
                }
                return;
            }
        }
    }

    // --- 7. ESC ---
    if (IsKeyPressed(KEY_ESCAPE)) {
        *currentScreen = MENU;
        CleanupGame();
        return;
    }

    // --- 8. DRAW ---
    gameScene->Draw();
    gamePlayer->Draw();

    // DK: solo dibujar enemy antiguo si NO está en Scene2 con DK caído/aterrizado
    bool drawOldDk = true;
    if (isScene2) {
        Scene2* scene2 = dynamic_cast<Scene2*>(gameScene);
        if (scene2 && (scene2->IsDkFalling() || scene2->IsDkLanded() || scene2->IsDkOnPlatform())) {
            drawOldDk = false;  // No dibujar el antiguo, el nuevo lo maneja Scene2
        }
    }
    if (drawOldDk) {
        gameEnemy->Draw();
    }

    gameStars->Draw();

    if (hammerActive && !hammerCollected) {
        DrawTextureEx(hammerTexture, hammerPosition, 0.0f, hammerScale, WHITE);
    }

    if (gamePlayer != nullptr) {
        DrawRectangle(10, 10, 120, 30, Fade(BLACK, 0.7f));
        DrawText(TextFormat("SCORE: %04d", gamePlayer->GetScore()), 15, 15, 20, WHITE);
    }

    if (shouldReset) {
        CleanupGame();
        shouldReset = false;
    }
}

void UnloadGame()
{
    CleanupGame();
}

void SwitchToScene2()
{
    InitGameScene2();
}

void ResetGame()
{
    shouldReset = true;
}

void SetHammerPosition(float x, float y) {
    hammerPosition = { x, y };
}