#pragma once
#include "raylib.h"
#include <vector>

class FireSprite
{
public:
    FireSprite(Vector2 startPos);
    ~FireSprite();

    void Update(float deltaTime);
    void Draw();
    Rectangle GetHitbox();

    // ===== 新增（平台范围）=====
    void SetRange(float minX, float maxX);

private:

    // ===== 动画（统一系统）=====
    std::vector<Texture2D> frames;
    int currentFrame = 0;
    float frameTimer = 0.0f;

    // ===== 位置 =====
    Vector2 position;

    // ===== 移动 =====
    float moveSpeed;
    int direction; // ✔ 统一使用这个（删除 moveLeft）

    // ===== 跳动 =====
    float jumpTimer;
    float jumpOffset;

    // ===== 平台范围 =====
    float minX, maxX;

    // ===== 基础Y（防止跳动累积）=====
    float baseY;
};