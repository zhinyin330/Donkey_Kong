#include "FireSprite.h"
#include "resource_dir.h" 
#include "raymath.h"
#include <cmath>

FireSprite::FireSprite(Vector2 startPos, FireAnimationType type)
{
    position = startPos;
    groundY = startPos.y;
    isActive = true;
    isDead = false;
    animationType = type;

    //  初始化
    isWaiting = true;
    waitTimer = 2.0f;
    blinkTimer = 0.0f;
    visible = true;

    //  移动范围限制 
    minX = 0;
    maxX = 800;

    //  随机初始方向 
    direction = (GetRandomValue(0, 1) == 0) ? -1 : 1;
    lastDirection = direction;

    //  跳跃参数初始化 
    jumpState = IDLE;
    idleTimer = 0.0f;
    jumpProgress = 0.0f;
    jumpTimer = 0.0f;
    frameAccumulator = 0.0f;
    // 根据类型加载动画帧
    LoadFramesByType();

    currentFrame = 0;
    frameTimer = 0.0f;
}

void FireSprite::LoadFramesByType()
{
    if (animationType == FireAnimationType::SCENE1)
    {
        // Scene 使用的贴图：Dk_FireSprite1, Dk_FireSprite2
        for (int i = 1; i <= 2; i++)
        {
            frames.push_back(
                LoadTexture(TextFormat("Characters/FireSprites/Dk_FireSprite%d.png", i))
            );
        }
    }
    else
    {
        // Scene2 使用的贴图：Dk_FireSprite_Jump1, Dk_FireSprite_Jump2
        for (int i = 1; i <= 2; i++)
        {
            frames.push_back(
                LoadTexture(TextFormat("Characters/FireSprites/Dk_FireSprite_Jump%d.png", i))
            );
        }
    }
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
    // 如果小火人死亡或未激活，不更新
    if (isDead || !isActive) return;
    // ===== 等待2秒期间（闪烁，不移动）=====
    if (isWaiting)
    {
        // 更新等待计时器
        waitTimer -= deltaTime;

        // 闪烁效果：每0.1秒切换一次可见性
        blinkTimer += deltaTime;
        if (blinkTimer >= 0.1f)
        {
            blinkTimer = 0.0f;
            visible = !visible;
        }

        // 等待结束，开始移动
        if (waitTimer <= 0.0f)
        {
            isWaiting = false;
            visible = true;
            StartNewJump();  // 开始第一次跳跃
        }
        return;  // 等待期间不执行移动逻辑
    }

    // ===== 以下是移动逻辑（只在非等待状态执行）=====
    // 限制deltaTime最大值，防止跳跃过大
    if (deltaTime > 0.033f) deltaTime = 0.033f;

    // 状态机处理
    switch (jumpState)
    {
        case IDLE:
            idleTimer += deltaTime;
            if (idleTimer >= idleDuration)
            {
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
                frameAccumulator = 0.0f;
            }
            else
            {
                Vector2 newPos = CalculateParabolaPosition(jumpProgress);
                position = newPos;
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

    //  随机决定下一次移动方向 
    // 70%几率随机改变方向，40%几率保持原方向
    if (GetRandomValue(0, 100) < 60)
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
    // 死亡或未激活时不绘制
    if (isDead || !isActive) return;

    

    // 等待期间闪烁效果（不可见时跳过绘制）
    if (isWaiting && !visible)
    {
        return;
    }

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
        roundf(position.x), 
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
    // 死亡或未激活时返回空碰撞箱
    if (isDead || !isActive) return { -100, -100, 0, 0 };
    if (isWaiting) return { -100, -100, 0, 0 };  // 等待期间无碰撞
    if (currentFrame >= 0 && currentFrame < (int)frames.size())
    {
        // 原始图片 16x16，绘制时放大2倍 = 32x32
        float drawnWidth = 32.0f;   // 16 * 2
        float drawnHeight = 32.0f;  // 16 * 2

        // 缩小碰撞箱（占绘制大小的50%）
        float hitboxWidth = 18.0f;   // 约56%的宽度
        float hitboxHeight = 20.0f;  // 约62%的高度

        // 计算偏移使碰撞箱居中并稍微偏下
        float offsetX = (drawnWidth - hitboxWidth) / 2.0f;      // 水平居中：(32-18)/2 = 7
        float offsetY = drawnHeight - hitboxHeight - 4.0f;       // 底部对齐，向上偏移4像素

        return {
            roundf(position.x + offsetX),
            roundf(position.y + offsetY),
            hitboxWidth,
            hitboxHeight
        };
    }
    return { position.x, position.y, 32.0f, 32.0f };
}

void FireSprite::SetRange(float minX, float maxX)
{
    this->minX = minX;
    this->maxX = maxX;
}

//  重置位置 
void FireSprite::ResetPosition(Vector2 newPos)
{
    position = newPos;
    groundY = newPos.y;

    // 重置跳跃状态
    jumpState = IDLE;
    idleTimer = 0.0f;
    jumpTimer = 0.0f;
    jumpProgress = 0.0f;
    frameAccumulator = 0.0f;
    currentFrame = 0;
    frameTimer = 0.0f;

    // 重置等待系统
    isWaiting = true;
    waitTimer = 2.0f;
    blinkTimer = 0.0f;
    visible = true;

    // 随机初始方向
    direction = (GetRandomValue(0, 1) == 0) ? -1 : 1;
    lastDirection = direction;

    // 激活小火人
    isActive = true;
    isDead = false;
}

//  新增：小火人死亡 
void FireSprite::Die()
{
    isDead = true;
    isActive = false;
    isWaiting = false;
}