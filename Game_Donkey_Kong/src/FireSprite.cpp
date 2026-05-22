#include "FireSprite.h"
#include "resource_dir.h" 
#include "raymath.h"
#include <cmath>

FireSprite::FireSprite(Vector2 startPos)
{
    position = startPos;
    groundY = startPos.y;           // 保存地面Y坐标

    // ===== 移动范围限制 =====
    minX = 0;
    maxX = 800;

    // ===== 随机初始方向 =====
    direction = (GetRandomValue(0, 1) == 0) ? -1 : 1;
    lastDirection = direction;

    // ===== 跳跃参数初始化 =====
    jumpState = IDLE;
    idleTimer = 0.0f;
    jumpProgress = 0.0f;
    jumpTimer = 0.0f;
    frameAccumulator = 0.0f;

    // ===== 加载动画帧 =====
    for (int i = 1; i <= 3; i++)
    {
        frames.push_back(
            LoadTexture(TextFormat("Characters/FireSprites/Dk_FireSprite%d.png", i))
        );
    }

    currentFrame = 0;
    frameTimer = 0.0f;
}

FireSprite::~FireSprite()
{
    // 卸载纹理
    for (auto& frame : frames) {
        UnloadTexture(frame);
    }
    frames.clear();
}

Vector2 FireSprite::CalculateParabolaPosition(float t)
{
    Vector2 result;

    // 使用 easing 函数使移动更平滑（缓入缓出效果）
    float smoothT = t * t * (3.0f - 2.0f * t);  // Smoothstep 函数

    // 水平移动
    result.x = jumpStartX + (jumpTargetX - jumpStartX) * smoothT;

    // 完整抛物线（使用正弦曲线使跳跃更自然）
    float parabolaY = sinf(t * PI) * jumpHeight;

    result.y = groundY - parabolaY;

    return result;
}

void FireSprite::Update(float deltaTime)
{
    // 限制deltaTime最大值，防止跳跃过大
    if (deltaTime > 0.033f) deltaTime = 0.033f;

    // ===== 状态机处理 =====
    switch (jumpState)
    {
    case IDLE:
        // 落地等待状态，结束后开始下一次跳跃
        idleTimer += deltaTime;
        if (idleTimer >= idleDuration)
        {
            // 开始新的跳跃
            StartNewJump();
        }
        break;

    case JUMPING:
    {
        jumpTimer += deltaTime;

        jumpProgress = jumpTimer / jumpDuration;

        if (jumpProgress >= 1.0f)
        {
            position.x = jumpTargetX;
            position.y = groundY;

            jumpState = IDLE;
            idleTimer = 0.0f;

            currentFrame = 0;
            frameTimer = 0.0f;
            frameAccumulator = 0.0f;  // ⭐ 新增：重置累积器
        }
        else
        {
            // ⭐ 修改：使用更平滑的抛物线计算
            Vector2 newPos = CalculateParabolaPosition(jumpProgress);
            position = newPos;

            // ⭐ 新增：在跳跃过程中始终更新动画
            UpdateAnimation(deltaTime);
        }
        break;
    }
    }
}
void FireSprite::StartNewJump()
{
    // 保存跳跃起始位置
    jumpStartX = position.x;
    jumpStartY = groundY;

    // ===== 随机决定下一次移动方向 =====
    // 70%几率随机改变方向，30%几率保持原方向
    if (GetRandomValue(0, 100) < 70)
    {
        direction = (GetRandomValue(0, 1) == 0) ? -1 : 1;
    }

    // 计算跳跃目标X坐标
    jumpTargetX = position.x + (direction * jumpDistance);

    // 边界限制：如果超出范围，则反弹方向
    if (jumpTargetX < minX)
    {
        jumpTargetX = minX;
        direction = 1;  // 反弹向右
    }
    else if (jumpTargetX > maxX)
    {
        jumpTargetX = maxX;
        direction = -1; // 反弹向左
    }

    // 根据边界修正后的目标位置重新确定方向
    if (jumpTargetX > position.x)
        direction = 1;
    else if (jumpTargetX < position.x)
        direction = -1;
    else
        direction = (GetRandomValue(0, 1) == 0) ? -1 : 1;  // 如果不动，随机选方向

    // 更新上一帧方向
    lastDirection = direction;

    // 重置跳跃变量
    jumpState = JUMPING;
    jumpTimer = 0.0f;
    jumpProgress = 0.0f;
}

void FireSprite::UpdateAnimation(float deltaTime)
{
    // 跳跃时循环播放动画
    frameAccumulator += deltaTime;
    while (frameAccumulator >= frameDelay) {
        frameAccumulator -= frameDelay;
        currentFrame = (currentFrame + 1) % (int)frames.size();
    }
}
void FireSprite::Draw()
{
    if (currentFrame < 0 || currentFrame >= (int)frames.size())
        return;

    Texture2D tex = frames[currentFrame];

    Rectangle sourceRect;

    int drawDirection = direction;

    if (direction == -1)
    {
        sourceRect = {
            (float)tex.width,
            0,
            -(float)tex.width,
            (float)tex.height
        };
    }
    else
    {
        sourceRect = {
            0,
            0,
            (float)tex.width,
            (float)tex.height
        };
    }

    Rectangle destRect = {
        roundf(position.x),  // 四舍五入到整数像素
        roundf(position.y),
        tex.width * 2.0f,
        tex.height * 2.0f
    };

    DrawTexturePro(
        tex,
        sourceRect,
        destRect,
        { 0, 0 },
        0.0f,
        WHITE
    );
}

Rectangle FireSprite::GetHitbox()
{
    if (currentFrame >= 0 && currentFrame < (int)frames.size())
    {
        return {
             roundf(position.x),  
            roundf(position.y),
            frames[currentFrame].width * 2.0f,
            frames[currentFrame].height * 2.0f
        };
    }
    return { position.x, position.y, 32.0f, 32.0f };
}

void FireSprite::SetRange(float minX, float maxX)
{
    this->minX = minX;
    this->maxX = maxX;
}