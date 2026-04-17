#pragma once
#include "raylib.h"
#include <vector>

// 敌人状态
enum class EnemyState {
    IDLE,
    BARREL_GRAB // 拿桶并转身
};

class Enemy {
private:
    // ===== 贴图 =====
    std::vector<Texture2D> dkWithBarrelTextures;  // 拿桶时的大金刚图片 [0:左侧, 1:正面, 2:右侧]
    std::vector<Texture2D> dkEmptyTextures;       // 空手时的大金刚图片 [0:左侧, 1:正面, 2:右侧]
    Texture2D barrelSide;            // 桶侧面图
    Texture2D barrelFront;           // 桶正面图

    // ===== 动画 =====
    int currentFrame;
    int animDirection;  // 新增：控制 0->1->2->1->0 循环
    bool hasBarrel;       // 新增：控制木桶是否显示
    float frameCounter;
    float frameSpeed;

    // ===== 位置 & 属性 =====
    Vector2 position;
    float scale;
    std::vector<Vector2> barrelOffsets;// 木桶位置修正
    // ===== 状态 =====
    EnemyState currentState;
    bool isGoingForward;     // true:去程(从左到右), false:回程(从右到左)

public:
    Enemy();
    ~Enemy();

    // ===== 核心逻辑 =====
    void Update();
    void Draw();

    // ===== 动画控制 =====
    void UpdateAnimation();
    void ChangeState(EnemyState newState);
    
    // Getter/Setter（如果需要）
    Vector2 GetPosition() const { return position; }
    void SetPosition(float x, float y) { position = { x, y }; }
};