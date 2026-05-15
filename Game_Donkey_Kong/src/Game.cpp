#include "Game.h"
#include "resource_dir.h"
#include "Player.h"
#include "Scene.h"
#include "Scene2.h"
#include "Enemy.h"
#include "NewMechanic.h"
#include "GameScene.h"
#include "Transition.h"
#include "PauseMenu.h"
#include "GameOver.h"
#include "LeaderBoard.h"

static GameScene* gameScene = nullptr;
static Player* gamePlayer = nullptr;
static Enemy* gameEnemy = nullptr;
static NewMechanic* gameStars = nullptr;
static bool isInitialized = false;
static bool shouldReset = false;
static bool isScene2 = false;
static Transition* gameTransition = nullptr;
static PauseMenu* pauseMenu = nullptr;
static GameOver* gameOver = nullptr;
static int currentLevel = 1;
static int totalScore = 0;
static int totalStars = 0;
static Texture2D hammerTexture = { 0 };
static Vector2 hammerPosition = { 400, 300 };
static bool hammerActive = true;
static bool hammerCollected = false;
static float hammerScale = 2.3f;
static LeaderBoard* leaderBoard = nullptr;
static bool gameInProgress = false;


void InitGame()
{
    CleanupGame();

    gameScene = new Scene();
    gamePlayer = new Player();
    gamePlayer->SetScore(totalScore);
    gameEnemy = new Enemy();
    gameEnemy->SetBehavior(EnemyBehavior::THROW_BARRELS);
    gameStars = new NewMechanic();
    gameTransition = new Transition();
    pauseMenu = new PauseMenu();
    gameOver = new GameOver();

    for (int i = 0; i < totalStars; i++) {
        gamePlayer->AddStar();
    }

    isScene2 = false;
    isInitialized = true;
    shouldReset = false;
    gameInProgress = true;

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
    gamePlayer->SetScore(totalScore);
    gameEnemy = new Enemy();
    gameEnemy->SetBehavior(EnemyBehavior::DECORATIVE_CYCLE);
    gameEnemy->SetPosition(340, 130);
    gameStars = new NewMechanic();
    pauseMenu = new PauseMenu();
    gameOver = new GameOver();

    for (int i = 0; i < totalStars; i++) {
        gamePlayer->AddStar();
    }

    isScene2 = true;
    isInitialized = true;
    shouldReset = false;
    gameTransition = new Transition();
    gameInProgress = true;

    hammerTexture = LoadTexture("items/Dk_Hammer_Up.png");
    hammerActive = true;
    hammerCollected = false;
    hammerPosition = { 400.0f, 300.0f };
}

void InitLeaderBoard() {
    if (leaderBoard == nullptr) {
        leaderBoard = new LeaderBoard();
    }
    leaderBoard->SetHasActiveGame(gameInProgress);
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
        if (pauseMenu != nullptr) { delete pauseMenu; pauseMenu = nullptr; }
        if (gameOver != nullptr) { delete gameOver; gameOver = nullptr; }
        if (leaderBoard != nullptr) { delete leaderBoard; leaderBoard = nullptr; }

        gameInProgress = false;
        isInitialized = false;
        isScene2 = false;
    }
}

void DrawLeaderBoard(GameScreen* currentScreen) {
    if (leaderBoard == nullptr) {
        InitLeaderBoard();
    }

    leaderBoard->Update();
    leaderBoard->Draw();

    if (IsKeyPressed(KEY_ENTER)) {
        int option = leaderBoard->GetSelectedOption();

        if (option == 0 && gameInProgress) {
            // Continuar partida - volver sin hacer nada
            *currentScreen = GAMEPLAY;
        }
        else {
            // Nueva partida
            totalScore = 0;
            totalStars = 0;
            currentLevel = 1;
            Player::ResetLives();
            // No borrar leaderBoard
            if (gameScene != nullptr) { delete gameScene; gameScene = nullptr; }
            if (gamePlayer != nullptr) { delete gamePlayer; gamePlayer = nullptr; }
            if (gameEnemy != nullptr) { delete gameEnemy; gameEnemy = nullptr; }
            if (gameStars != nullptr) { delete gameStars; gameStars = nullptr; }
            if (gameTransition != nullptr) { delete gameTransition; gameTransition = nullptr; }
            if (pauseMenu != nullptr) { delete pauseMenu; pauseMenu = nullptr; }
            if (gameOver != nullptr) { delete gameOver; gameOver = nullptr; }
            isInitialized = false;
            isScene2 = false;
            gameInProgress = false;

            InitGame();
            gameInProgress = true;
            *currentScreen = GAMEPLAY;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE) && gameInProgress) {
        *currentScreen = GAMEPLAY;
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
        if (pauseMenu == nullptr) pauseMenu = new PauseMenu();
        if (gameOver == nullptr) gameOver = new GameOver();

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
            // 调用炸弹更新逻辑
            scene2->UpdateBombs(deltaTime,gamePlayer);

            if (scene2->IsDkFalling() && !scene2->IsDkLanded()) {
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

    // --- 3. TRANSICIÓN ---
    if (gameScene->IsTransitionReached()) {
        if (gameTransition != nullptr && !gameTransition->IsFinished()) {
            gameTransition->Update();
            gameScene->Draw();
            gamePlayer->Draw();
            gameEnemy->Draw();
            gameStars->Draw();
            gameTransition->Draw();

            if (gameTransition->IsFinished()) {
                if (!isScene2) InitGameScene2();
                else InitGame();
            }
            return;
        }
    }

    // --- 4. PAUSA ---
    if (IsKeyPressed(KEY_P)) {
        if (pauseMenu != nullptr && !pauseMenu->IsActive()) {
            pauseMenu->Show();
        }
    }

    if (pauseMenu != nullptr && pauseMenu->IsActive()) {
        pauseMenu->Update();
        gameScene->Draw();
        gamePlayer->Draw();
        gameEnemy->Draw();
        gameStars->Draw();
        pauseMenu->Draw();

        if (IsKeyPressed(KEY_ENTER)) {
            if (pauseMenu->GetSelectedOption() == PauseOption::MAIN_MENU) {
                *currentScreen = LEADERBOARD;
                InitLeaderBoard();
                leaderBoard->SetHasActiveGame(true);
                pauseMenu->Hide();
                return;
            }
            else {
                pauseMenu->Hide();
            }
        }
        return;
    }

    // --- 5. MUERTE DE MARIO ---
    if (gamePlayer->IsDying()) {
        static bool barrelsCleared = false;
        if (!barrelsCleared && gameEnemy != nullptr) {
            gameEnemy->ClearBarrels();
            barrelsCleared = true;
        }

        gamePlayer->UpdateDeath(deltaTime);
        gameScene->Draw();
        gamePlayer->Draw();
        gameEnemy->Draw();
        gameStars->Draw();

        if (!gamePlayer->IsDying()) {
            barrelsCleared = false;
            gameStars->ResetStars();

            // Al terminar la animación, comprobar si murió
            if (gamePlayer->IsDead()) {
                *currentScreen = GAME_OVER;
                if (gameOver != nullptr) gameOver->Show();
            }
            else {
                gamePlayer->Respawn(2, 21);
            }
        }
        return;
    }

    if (gamePlayer->IsDead()) {
        *currentScreen = GAME_OVER;
        if (gameOver != nullptr) gameOver->Show();
        return;
    }

    // --- 6. UPDATES ---
    gamePlayer->HandleInput(*gameScene);
    gamePlayer->Update(*gameScene);
    gameEnemy->Update(*gameScene);
    gameStars->Update(deltaTime, GameScene::GetScreenWidth());
    gameStars->CheckCollisionWithPlayer(gamePlayer);

    // 桶碰油桶
    Scene* scene1 = dynamic_cast<Scene*>(gameScene);
    if (scene1 != nullptr) {
        scene1->UpdateOilCanisters(deltaTime);
        std::vector<OilCanister>& cans = scene1->GetOilCanisters();

        for (auto& barrel : gameEnemy->GetBarrels()) {
            if (!barrel.IsHit()) {
                Rectangle barrelRect = { barrel.GetPosition().x, barrel.GetPosition().y, barrel.GetWidth(), barrel.GetHeight() };
                for (int i = 0; i < (int)cans.size(); i++) {
                    if (cans[i].isActive && !cans[i].isBurning && CheckCollisionRecs(barrelRect, cans[i].rect)) {
                        barrel.Hit();
                        cans[i].isActive = false;
                        cans[i].isBurning = true;
                        cans[i].burnTimer = 0;
                        break;
                    }
                }
            }
        }
    }

    // Martillo
    if (hammerActive && !hammerCollected && gamePlayer != nullptr) {
        Rectangle hammerHitbox = {
            hammerPosition.x, hammerPosition.y,
            hammerTexture.width * hammerScale, hammerTexture.height * hammerScale
        };
        Rectangle playerHitbox = gamePlayer->GetHitbox();
        if (CheckCollisionRecs(playerHitbox, hammerHitbox)) {
            hammerCollected = true;
            hammerActive = false;
            gamePlayer->SetHasHammer(true);
        }
    }

    // Martillo golpea barriles
    if (gamePlayer != nullptr && gamePlayer->IsSwingingHammer() && gameEnemy != nullptr) {
        Rectangle attackHitbox = gamePlayer->GetAttackHitbox();
        for (auto& barrel : gameEnemy->GetBarrels()) {
            if (!barrel.IsHit()) {
                Rectangle barrelHitbox = {
                    barrel.GetPosition().x, barrel.GetPosition().y,
                    barrel.GetWidth(), barrel.GetHeight()
                };
                if (CheckCollisionRecs(attackHitbox, barrelHitbox)) {
                    barrel.Hit();
                    int addPoints = (barrel.GetType() == BarrelType::NORMAL) ? 100 : 500;
                    gamePlayer->AddScore(addPoints);
                }
            }
        }
    }

    // Colisiones Scene 1 (barriles + DK)
    if (!isScene2 && gameEnemy != nullptr && !gamePlayer->IsInStarMode()) {
        Rectangle playerHitbox = gamePlayer->GetHitbox();

        for (auto& barrel : gameEnemy->GetBarrels()) {
            if (!barrel.IsHit()) {
                Rectangle barrelRect = {
                    barrel.GetPosition().x, barrel.GetPosition().y,
                    barrel.GetWidth(), barrel.GetHeight()
                };
                if (CheckCollisionRecs(playerHitbox, barrelRect)) {
                    barrel.Hit();
                    gamePlayer->LoseLife();
                    gameEnemy->ClearBarrels();
                    gamePlayer->StartDeath();
                    if (gamePlayer->IsDead()) {
                        *currentScreen = GAME_OVER;
                        if (gameOver != nullptr) gameOver->Show();
                        return;
                    }
                    break;
                }
            }
        }

        Rectangle dkHitbox = {
            gameEnemy->GetPosition().x, gameEnemy->GetPosition().y,
            gameEnemy->GetWidth(), gameEnemy->GetHeight()
        };
        if (CheckCollisionRecs(playerHitbox, dkHitbox)) {
            gamePlayer->LoseLife();
            gameEnemy->ClearBarrels();
            gamePlayer->StartDeath();
            if (gamePlayer->IsDead()) {
                *currentScreen = GAME_OVER;
                if (gameOver != nullptr) gameOver->Show();
                return;
            }

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
                gamePlayer->GetPosition().x, gamePlayer->GetPosition().y,
                gamePlayer->GetTextureWidth() * gamePlayer->GetScale(),
                gamePlayer->GetTextureHeight() * gamePlayer->GetScale()
            };
            scene2->CheckButtonCollision(fullHitbox, gamePlayer);
        }
    }

    // Colisión con DK en Scene 2
    if (isScene2 && gameEnemy != nullptr && !gamePlayer->IsInStarMode()) {
        Scene2* scene2 = dynamic_cast<Scene2*>(gameScene);
        if (scene2 && scene2->CanDkHurt()) {
            Rectangle playerHitbox = gamePlayer->GetHitbox();
            Rectangle dkHitbox = {
                gameEnemy->GetPosition().x, gameEnemy->GetPosition().y,
                gameEnemy->GetWidth(), gameEnemy->GetHeight()
            };
            if (CheckCollisionRecs(playerHitbox, dkHitbox)) {
                gamePlayer->LoseLife();
                gamePlayer->StartDeath();
                if (gamePlayer->IsDead()) {
                    *currentScreen = GAME_OVER;
                    if (gameOver != nullptr) gameOver->Show();
                    return;
                }

            }
        }
    }

    // --- 7. Tecla T ---
    if (IsKeyPressed(KEY_T) && !isScene2) {
        InitGameScene2();
        return;
    }

    // --- 8. Princesa Scene 1 ---
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
                totalScore = gamePlayer->GetScore();
                gameScene->SetTransitionReached(true);
                if (gameTransition != nullptr) {
                    gameTransition->Start(currentLevel, totalScore, totalStars);
                }
                return;
            }
        }
    }

    // --- 9. Princesa Scene 2 ---
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
                totalScore = gamePlayer->GetScore();
                currentLevel++;
                gameScene->SetTransitionReached(true);
                if (gameTransition != nullptr) {
                    gameTransition->Start(currentLevel, totalScore, totalStars);
                }
                return;
            }
        }
    }

    // --- 10. DRAW ---
    gameScene->Draw();
    gamePlayer->Draw();

    bool drawOldDk = true;
    if (isScene2) {
        Scene2* scene2 = dynamic_cast<Scene2*>(gameScene);
        if (scene2 && (scene2->IsDkFalling() || scene2->IsDkLanded() || scene2->IsDkOnPlatform())) {
            drawOldDk = false;
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

void DrawGameOver(GameScreen* currentScreen) {
    if (gameOver == nullptr) return;

    gameOver->Update();
    gameOver->Draw();

    if (gameOver->IsFinished()) {
        std::string name = gameOver->GetPlayerName();
        int finalLevel = currentLevel;
        int finalScore = totalScore;

        // Limpiar juego pero NO el leaderBoard
        if (gameScene != nullptr) { delete gameScene; gameScene = nullptr; }
        if (gamePlayer != nullptr) { delete gamePlayer; gamePlayer = nullptr; }
        if (gameEnemy != nullptr) { delete gameEnemy; gameEnemy = nullptr; }
        if (gameStars != nullptr) { delete gameStars; gameStars = nullptr; }
        if (gameTransition != nullptr) { delete gameTransition; gameTransition = nullptr; }
        if (pauseMenu != nullptr) { delete pauseMenu; pauseMenu = nullptr; }
        if (gameOver != nullptr) { delete gameOver; gameOver = nullptr; }
        isInitialized = false;
        isScene2 = false;
        gameInProgress = false;

        // Ahora añadir score (leaderBoard sigue vivo)
        InitLeaderBoard();
        if (leaderBoard != nullptr) {
            leaderBoard->AddScore(name, finalLevel, finalScore);
            leaderBoard->SetHasActiveGame(false);
        }

        // Resetear para nueva partida
        totalScore = 0;
        totalStars = 0;
        currentLevel = 1;
        Player::ResetLives();

        *currentScreen = LEADERBOARD;
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