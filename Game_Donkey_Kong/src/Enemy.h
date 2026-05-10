#pragma once
#include "raylib.h"
#include <vector>
#include "Barrel.h" 

class GameScene;

// 敌人状态
enum class EnemyState {
    IDLE,
    BARREL_GRAB 
};

// (^^)d
enum class EnemyBehavior {
    THROW_BARRELS,   // Scene 1: Lanza barriles
    STATIONARY,       // Scene 2: Solo baila, no hace nada
    DECORATIVE_CYCLE  // Scene 2
};

class Enemy {
private:
    // ===== 贴图 =====
    Texture2D currentTexture;
    std::vector<Texture2D> dkWithBarrelTextures;  // 拿桶时的大金刚图片
    std::vector<Texture2D> dkEmptyTextures;       // 空手时的大金刚图片
    Texture2D idleTexture;
    Texture2D emote1Texture;
    Texture2D emote2Texture;

    // ===== 动画 =====
    int currentFrame;
    int animDirection;  
    bool hasBarrel;       // 木桶是否拿着
    float frameCounter;
    float frameSpeed;

    // ===== 位置 & 属性 =====
    Vector2 position;
    float scale;
    
    // ===== 状态 =====
    EnemyState currentState;
    bool isGoingForward;     // true:去程(从左到右), false:回程(从右到左)
    EnemyBehavior behavior; // (^^)d

    // ===== 新增：当前桶类型
    BarrelType currentBarrelType;
    // 列表
    std::vector<Barrel> barrels;

    void SpawnBarrel(); // 丢桶
    void UpdateDecorativeCycle();

public:
    Enemy();
    ~Enemy();

    // ===== 核心逻辑 =====
    void Update(GameScene& scene);
    void Draw();

    // ===== 动画控制 =====
    void UpdateAnimation();
    void ChangeState(EnemyState newState);

    // (^^)d: comportamiento
    void SetBehavior(EnemyBehavior b) { behavior = b; }
    EnemyBehavior GetBehavior() const { return behavior; }
    
    Vector2 GetPosition() const { return position; }
    void SetPosition(float x, float y) { position = { x, y }; }

    //获取位置和尺寸的方法}
    float GetScale() const { return scale; }
    float GetTextureWidth() const { return currentTexture.width; }
    float GetTextureHeight() const { return currentTexture.height; }

    const std::vector<Barrel>& GetBarrels() const { return barrels; }
    std::vector<Barrel>& GetBarrels() { return barrels; }  // 返回引用以便修改
};