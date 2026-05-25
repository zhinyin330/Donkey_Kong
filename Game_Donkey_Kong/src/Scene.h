#pragma once
#include "raylib.h"
#include <vector>
#include "GameScene.h"
#include "Princess.h"

class Scene : public GameScene {
private:

    // hurry音乐
    Music hurryMusic;        // 紧急音乐
    bool hurryMusicPlaying;  // 是否正在播放
    bool hurryMusicTriggered; // 是否已触发

    static const int tileSize = 16;
    static const int tileScale = 2;
    static const int mapWidth = 25;
    static const int mapHeight = 22;

    std::vector<std::vector<int>> level;
    std::vector<std::vector<int>> hitboxLevel;
    std::vector<std::vector<int>> ladderLevel;
    std::vector<std::vector<int>> ladderHitbox;
    std::vector<std::vector<int>> visualOffsetY;

    std::vector<OilCanister> oilCanisters;
    std::vector<Texture2D> fireFrames;

    Texture2D tileTexture;
    Texture2D ladderTexture;
    Texture2D barrelTexture;
    Texture2D oilCanisterTexture;

    Music backgroundMusic;

    Texture2D texHighScore;
    Texture2D texScore;
    Texture2D Level;
    Texture2D starIcon;

    Princess princess; 

    static const int platformHitboxHeight = 8;
    static const int platformHitboxOffsetY = 8;

    void AddLadder(int startY, int endY, int x,
        const std::vector<int>& visual,
        const std::vector<int>& hitboxes);

    Rectangle transitionZone;
    bool transitionReached;

    int currentLevel = 1;
    int GetCurrentLevel() override { return currentLevel; }
    void SetCurrentLevel(int level) override { currentLevel = level; }

    struct VerticalDropTrigger {
        float x;
        float y;
    };

    std::vector<VerticalDropTrigger> verticalDropTriggers;

    Texture2D rainTextures[4];
    int currentRainFrame;
    float rainTimer;
    float rainFrameSpeed;

    float sceneTimer;
    float sceneTimeLimit;
    Texture2D timeTexture;
    Rectangle timeSourceRec;
    float timeScale;

public:
    Scene();
    ~Scene();

    void Draw() override;
    bool IsSolid(int x, int y) override;
    bool IsLadder(int x, int y) override;
    int GetLadderType(int x, int y) override;
    int GetLadderHitbox(int x, int y) override;
    int GetVisualOffsetY(int x, int y) override;
    int GetTileSize() override { return tileSize * tileScale; }
    int GetPlatformHitboxHeight() override { return platformHitboxHeight * tileScale; }
    int GetPlatformHitboxOffsetY() override { return platformHitboxOffsetY * tileScale; }
    Vector2 GetPrincessPosition() override { return princess.GetPosition(); }
    float GetPrincessScale() override { return princess.GetScale(); }
    void UpdatePrincess(float deltaTime);

    Rectangle GetTransitionZone() override { return transitionZone; }
    bool IsTransitionReached() override { return transitionReached; }
    void SetTransitionReached(bool val) override { transitionReached = val; }

    void UpdateOilCanisters(float deltaTime);
    std::vector<OilCanister>& GetOilCanisters() { return oilCanisters; }

    void UpdateTimer(float deltaTime);
    float GetTimeLeft() { return sceneTimeLimit - sceneTimer; }
    void ResetTimer() { sceneTimer = 0.0f; }
};