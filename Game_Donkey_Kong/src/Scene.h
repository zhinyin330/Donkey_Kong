#pragma once
#include "raylib.h"
#include <vector>
#include "GameScene.h"
#include "Princess.h"
#include "FireSprite.h" 
#include "Barrel.h" 

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

    // ===== 新增：火人相关成员 =====
    std::vector<FireSprite*> fireSprites;      // 火人列表
    int blueBarrelsThrownCount;                // 已扔进油桶的蓝色桶数量
    static const int MAX_BLUE_BARREL_THROWS = 8;  // 最多扔2次

    // ===== 平台信息结构 =====
    struct PlatformInfo {
        int tileY;           // 平台地面的 Y 坐标
        float minX;        // 最小 X 范围
        float maxX;        // 最大 X 范围
        int platformIndex; // 平台索引 (1-5)
    };

    std::vector<PlatformInfo> fireSpawnPlatforms;  // 火人生成平台列表

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


    // ===== 新增：火人相关方法 =====
    void UpdateFireSprites(float deltaTime);
    void DrawFireSprites();
    void SpawnFireSprite(Vector2 position);
    void CheckBarrelOilCanCollision(class Barrel* barrel);  // 前向声明，需要包含 Barrel.h
    void CleanupFireSprites();
    std::vector<FireSprite*>& GetFireSprites() { return fireSprites; }
    int GetBlueBarrelsThrownCount() const { return blueBarrelsThrownCount; }
    // 获取指定位置的平台地面高度（考虑倾斜）
    float GetPlatformGroundY(float x, int platformYLevel);
    void InitFireSpawnPlatforms(); 
};