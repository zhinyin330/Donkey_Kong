#pragma once
#include "raylib.h"
#include <vector>
#include "GameScene.h"
#include "Player.h"
#include "FireSprite.h"
#include "Princess.h"

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
    //baozha.audio
    Sound bombExplosionSound; 
    bool bombSoundLoaded;
    Sound fireKillSound;        // 小火人被击杀音效
    bool fireKillSoundLoaded = false;

    // DK 掉落音效
    Sound dkFallSoundEffect;      // 掉落音效（与背景音乐区分）
    bool dkFallSoundEffectLoaded;
    bool hasPlayedFallSound;

//happy eding
    Sound dkFallSound;       
    bool dkFallSoundLoaded = false;

    // princesa
    Princess princess;

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
    
    //炸弹相关变量
    struct Bomb {
        Vector2 position;
        int currentFrame;
        float frameTimer;
        bool active;
        int stage;          // 0 代表第一段(帧0-2)，1 代表第二段(帧3-5)
        int loopCount;      // 记录当前阶段已经循环播放了几次
        bool isTriggered; //标记是否已被玩家靠近
        float lifeTimer;//存在时间
    };
    std::vector<Texture2D> bombTextures; // 存储 Bomb1 到 Bomb6
    std::vector<Bomb> activeBombs;       // 当前屏幕上的炸弹
    float bombSpawnTimer;                // 距离下一次生成炸弹的时间
    float nextSpawnTime;                 // 随机生成的间隔目标（2-7秒）

    Texture2D snowTextures[6];
    int currentSnowFrame;
    float snowTimer;
    float snowFrameSpeed;

    float sceneTimer;
    float sceneTimeLimit;
    Texture2D timeTexture;
    Rectangle timeSourceRec;
    float timeScale;
    // ===== 小火人管理系统 =====
    std::vector<FireSprite*> fireSprites;  // 存储所有小火人
    float fireSpawnTimer;                   // 生成计时器
    float fireSpawnInterval = 10.0f;       // 生成间隔（秒）
    int maxFireSprites = 8;                // 最大同时存在的小火人数量
    // 平台位置数据（从下往上数4个平台）
    struct PlatformInfo {
        float y;           // Y坐标
        float minX;        // 最小X
        float maxX;        // 最大X
    };
    std::vector<PlatformInfo> platforms;

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
    Vector2 GetPrincessPosition() override { return princess.GetPosition(); }
    float GetPrincessScale() override { return princess.GetScale(); }
    void UpdatePrincess(float deltaTime);
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
    void UpdateTimer(float deltaTime);
    float GetTimeLeft() { return sceneTimeLimit - sceneTimer; }
    void ResetTimer() { sceneTimer = 0.0f; }

    // Métodos de transición
    bool transitionReached = false;
    Rectangle GetTransitionZone() override { return { 0, 0, 0, 0 }; }
    bool IsTransitionReached() override { return transitionReached; }
    void SetTransitionReached(bool val) override { transitionReached = val; }
    bool CheckAdjacentTiles() override { return false; }

    void UpdateMusic();

    //更新炸弹
    void UpdateBombs(float deltaTime, Player* player);

    // 火焰敌人
    // ===== 新增：生成新小火人 =====
    void SpawnFireSprite();

    // ===== 新增：清理所有小火人 =====
    void ClearAllFireSprites();
    void Update(float deltaTime, Player* player);

};