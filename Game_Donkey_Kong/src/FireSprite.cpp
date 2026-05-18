#include "FireSprite.h"
#include "resource_dir.h" 
#include "raymath.h"
#include <cmath>
FireSprite::FireSprite(Vector2 startPos)
{
    position = startPos;

    moveSpeed = 80.0f;

    jumpTimer = 0.0f;
    jumpOffset = 0.0f;

    baseY = startPos.y;

    minX = 0;
    maxX = 800;

    direction = (GetRandomValue(0, 1) == 0) ? -1 : 1;

    // ===== 加载动画帧 =====
    for (int i = 1; i <= 3; i++)
    {
        frames.push_back(
            LoadTexture(TextFormat("Characters/FireSprites/Dk_FireSprite%d.png", i))
        );
    }
}

FireSprite::~FireSprite()
{

}
void FireSprite::Update(float deltaTime)
{
    // ===== 1. 左右移动 =====
    position.x += moveSpeed * direction * deltaTime;
    TraceLog(LOG_INFO, "Fire X: %f", position.x);
    // 边界反弹
    if (position.x < minX)
    {
        position.x = minX;
        direction = 1;
    }
    if (position.x > maxX)
    {
        position.x = maxX;
        direction = -1;
    }

    // ===== 2. 动画 =====
    frameTimer += deltaTime;
    if (frameTimer > 0.15f)
    {
        frameTimer = 0;
        currentFrame = (currentFrame + 1) % frames.size();
    }

    // ===== 3. 跳动（修复版）=====
    jumpTimer += deltaTime;
    jumpOffset = sin(jumpTimer * 6.0f) * 4.0f;

    position.y = baseY + jumpOffset;
}
void FireSprite::Draw()
{
    DrawTextureEx(
        frames[currentFrame],
        position,
        0.0f,
        2.0f,
        WHITE
    );
}

Rectangle FireSprite::GetHitbox()
{
    return {
        position.x,
        position.y,
        frames[currentFrame].width * 2.0f,
        frames[currentFrame].height * 2.0f
    };
}