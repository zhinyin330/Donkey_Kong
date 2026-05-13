#pragma once
#include "raylib.h"
#include <vector>
#include "GameScene.h"
class Player;

class Scene2 : public GameScene {
private:
    static const int tileSize = 16;
    static const int tileScale = 2;
    static const int mapWidth = 25;
    static const int mapHeight = 22;

    std::vector<std::vector<int>> level;
    std::vector<std::vector<int>> hitboxLevel;
    std::vector<std::vector<int>> ladderLevel;
    std::vector<std::vector<int>> ladderHitbox;
    std::vector<Vector2> pillars;
    std::vector<Vector2> buttons;
    std::vector<Rectangle> newPlatforms;
    std::vector<Texture2D> dkFallFrames;  // ← Solo esto, quitamos el array

    Texture2D tileTexture;
    Texture2D ladderTexture;
    Texture2D pillarTexture;
    Texture2D buttonTexture;

    // 背景音乐
    Music backgroundMusic;

    // 加分物品
    Texture2D item1Texture;
    Texture2D item2Texture;
    Texture2D item3Texture;
    Vector2 item1Pos;
    Vector2 item2Pos;
    Vector2 item3Pos;
    bool item1Active;
    bool item2Active;
    bool item3Active;
    std::vector<bool> buttonsActive;
    std::vector<bool> buttonsScored;

    // princesa
    Texture2D princessTexture;
    Vector2 princessPosition;
    float princessScale;

    //donkey died
    bool dkFalling;
    float dkFallTimer;
    int dkFallFrame;
    float dkFallSpeed;
    Vector2 dkStartPosition;
    bool dkLanded;
    bool dkOnPlatform;
    float dkBounceTimer;
    int dkBounceFrame;
    bool dkCanHurt;

    static const int platformHitboxHeight = 8;
    static const int platformHitboxOffsetY = 8;

    void AddLadder(int startY, int endY, int x,
        const std::vector<int>& visual,
        const std::vector<int>& hitboxes);
    void AddPillar(float pixelX, float pixelY);

    //level
    int currentLevel = 1;
    int GetCurrentLevel() override { return currentLevel; }
    void SetCurrentLevel(int level) override { currentLevel = level; }

    void CheckPlatformsStatus();
    bool newPlatformsVisible;
    bool dkSequenceDone;
    bool sequenceTriggered;

public:
    Scene2();
    ~Scene2();

    void Draw() override;
    void CheckItemCollision(Rectangle playerHitbox, Player* player);
    bool IsSolid(int x, int y) override;
    bool IsLadder(int x, int y) override;
    int GetLadderType(int x, int y) override;
    int GetLadderHitbox(int x, int y) override;
    int GetVisualOffsetY(int x, int y) override { return platformHitboxOffsetY * tileScale; }
    int GetTileSize() override { return tileSize * tileScale; }
    int GetPlatformHitboxHeight() override { return platformHitboxHeight * tileScale; }
    int GetPlatformHitboxOffsetY() override { return platformHitboxOffsetY * tileScale; }
    Vector2 GetPrincessPosition() override { return princessPosition; }
    float GetPrincessScale() override { return princessScale; }
    void CheckButtonCollision(Rectangle playerHitbox, Player* player);
    bool CheckNewPlatformCollision(Rectangle playerFeetHitbox, float& groundY) override;
    bool HasNewPlatforms() override { return newPlatformsVisible; }
    std::vector<Rectangle> GetNewPlatforms() override { return newPlatforms; }
    bool IsDkFalling() { return dkFalling; }
    bool IsDkLanded() { return dkLanded; }
    bool IsDkOnPlatform() { return dkOnPlatform; }
    Vector2 GetDkPosition() { return dkStartPosition; }
    float GetDkScale() { return 2.8f; }
    Texture2D GetDkCurrentTexture();
    void UpdateDkFall(float deltaTime);
    void UpdateDkBounce(float deltaTime);
    void DrawDkFalling();
    void TriggerDkLandSequence();
    bool CanDkHurt() { return dkCanHurt; }

    // Métodos de transición
    bool transitionReached = false;
    Rectangle GetTransitionZone() override { return { 0, 0, 0, 0 }; }
    bool IsTransitionReached() override { return transitionReached; }
    void SetTransitionReached(bool val) override { transitionReached = val; }
    bool CheckAdjacentTiles() override { return false; }

    void UpdateMusic();
};