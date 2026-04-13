#pragma once
#include "raylib.h"
#include <vector>

// 敌人状态（目前只有一个动画，后面可以扩展）
enum class EnemyState {
    BARREL_GRAB
};

class Enemy {
private:
    // ===== 贴图 =====
    std::vector<Texture2D> barrelGrabTextures;
    Texture2D currentTexture;

    // ===== 动画 =====
    EnemyState currentState;
    int currentFrame;
    float frameCounter;
    float frameSpeed;

    // ===== 位置 & 属性 =====
    Vector2 position;
    float scale;
    bool facingRight;

public:
    Enemy();
    ~Enemy();

    // ===== 核心逻辑 =====
    void Update();
    void Draw();

    // ===== 动画控制 =====
    void UpdateAnimation();
    void ChangeState(EnemyState newState);
};