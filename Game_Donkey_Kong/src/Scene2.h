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

    Texture2D tileTexture;
    Texture2D ladderTexture;
    Texture2D pillarTexture;
    Texture2D buttonTexture;

    // 背景音乐
    Music backgroundMusic;

    // 加分物品
    Texture2D item1Texture;   // Dk_Item1
    Texture2D item3Texture;   // Dk_Item3
    Vector2 item1Pos;         // Item1 位置
    Vector2 item3Pos;         // Item3 位置
    bool item1Active;
    bool item3Active;
    std::vector<bool> buttonsActive;
   
    // princesa
    Texture2D princessTexture;
    Vector2 princessPosition;
    float princessScale;

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

public:
    Scene2();
    ~Scene2();

    void Draw() override;
    void CheckItemCollision(Rectangle playerHitbox, Player* player);  // 新增
    bool IsSolid(int x, int y) override;
    bool IsLadder(int x, int y) override;
    int GetLadderType(int x, int y) override;
    int GetLadderHitbox(int x, int y) override;
    int GetVisualOffsetY(int x, int y) override { return platformHitboxOffsetY * tileScale;
    }
    int GetTileSize() override { return tileSize * tileScale; }
    int GetPlatformHitboxHeight() override { return platformHitboxHeight * tileScale; }
    int GetPlatformHitboxOffsetY() override { return platformHitboxOffsetY * tileScale;}
    Vector2 GetPrincessPosition() override { return princessPosition; }
    float GetPrincessScale() override { return princessScale; }
    void CheckButtonCollision(Rectangle playerHitbox);

    // Métodos de transición (aunque no se usen, deben estar)
    bool transitionReached = false;
    Rectangle GetTransitionZone() override { return { 0, 0, 0, 0 }; }
    bool IsTransitionReached() override { return transitionReached; }
    void SetTransitionReached(bool val) override { transitionReached = val; }
    bool CheckAdjacentTiles() override { return false; }

    void UpdateMusic();
};