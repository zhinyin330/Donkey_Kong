#pragma once
#include "raylib.h"
#include <vector>
#include "GameScene.h"

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

    Texture2D tileTexture;
    Texture2D ladderTexture;
    Texture2D pillarTexture;


    static const int platformHitboxHeight = 8;
    static const int platformHitboxOffsetY = 8;

    void AddLadder(int startY, int endY, int x,
        const std::vector<int>& visual,
        const std::vector<int>& hitboxes);
    void AddPillar(float pixelX, float pixelY);

public:
    Scene2();
    ~Scene2();

    void Draw() override;
    bool IsSolid(int x, int y) override;
    bool IsLadder(int x, int y) override;
    int GetLadderType(int x, int y) override;
    int GetLadderHitbox(int x, int y) override;
    int GetVisualOffsetY(int x, int y) override { return platformHitboxOffsetY * tileScale;
    }
    int GetTileSize() override { return tileSize * tileScale; }
    int GetPlatformHitboxHeight() override { return platformHitboxHeight * tileScale; }
    int GetPlatformHitboxOffsetY() override { return platformHitboxOffsetY * tileScale;}

    // Métodos de transición (aunque no se usen, deben estar)
    Rectangle GetTransitionZone() override { return { 0, 0, 0, 0 }; }
    bool IsTransitionReached() override { return false; }
    void SetTransitionReached(bool val) override {}
    bool CheckAdjacentTiles() override { return false; }
};