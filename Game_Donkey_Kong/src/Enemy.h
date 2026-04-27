#pragma once
#include "raylib.h"
#include <vector>
#include "Barrel.h" 

// 敌人状态
enum class EnemyState {
    IDLE,
    BARREL_GRAB 
};

class Scene;

class Enemy {
private:
    // ===== 贴图 =====
    std::vector<Texture2D> dkWithBarrelTextures;  // 拿桶时的大金刚图片
    std::vector<Texture2D> dkEmptyTextures;       // 空手时的大金刚图片

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
    // ===== 新增：当前桶类型
    BarrelType currentBarrelType;
    // 列表
    std::vector<Barrel> barrels;

private:
    void SpawnBarrel(); // 丢桶
public:
    Enemy();
    ~Enemy();

    // ===== 核心逻辑 =====
    void Update(Scene& scene);
    void Draw();

    // ===== 动画控制 =====
    void UpdateAnimation();
    void ChangeState(EnemyState newState);
    
    Vector2 GetPosition() const { return position; }
    void SetPosition(float x, float y) { position = { x, y }; }
};