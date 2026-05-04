// Barrel.cpp

#include "Barrel.h"
#include "GameScene.h"
#define MAP_HEIGHT 22

Barrel::Barrel(BarrelType t, Vector2 pos)
{
    type = t;
    position = pos;

    speed = 80.0f;
    movingRight = true;
    state = BarrelState::ROLLING;

    currentFrame = 0;
    frameCounter = 0.0f;
    frameSpeed = 0.1f;

    groundOffset = 35.0f;

    ladderRollSpeed = 120.0f;          // 下落速度
    // 梯子状态初始化
    currentLadderX = -1;
    currentLadderY = -1;
    ladderProgress = 0.0f;
    targetPlatformY = -1;              // 目标平台Y坐标

    LoadFrames();
}

void Barrel::LoadFrames()
{
    if (type == BarrelType::NORMAL)
    {
        // 普通滚动动画
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov1.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov2.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov3.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov4.png"));

        // 掉落/梯子动画
        fallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Fall1.png"));
        fallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Fall2.png"));
        ladderFrames = fallingFrames;
    }
    else  // BLUE_BARREL
    {
        // 蓝色桶滚动动画
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov1.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov2.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov3.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov4.png"));

        // 蓝色桶掉落/梯子动画
        fallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Fall1.png"));
        fallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Fall2.png"));
        ladderFrames = fallingFrames;
    }

    currentAnimationFrames = rollingFrames;
}

void Barrel::UpdateAnimation()
{
    frameCounter += GetFrameTime();

    if (frameCounter >= frameSpeed)
    {
        frameCounter = 0.0f;
        currentFrame = (currentFrame + 1) % currentAnimationFrames.size();
    }
}

// ============================================================
// FindNextPlatformBelow 的唯一实现
// ============================================================
int Barrel::FindNextPlatformBelow(GameScene& scene, int ladderX, int startY)
{
    int tileSize = scene.GetTileSize();

    TraceLog(LOG_INFO, "Searching for platform below ladder at X=%d, starting from Y=%d", ladderX, startY);

    // 从梯子入口的下方开始查找
    for (int y = startY + 1; y < MAP_HEIGHT; y++)
    {
        // 检查当前位置是否是固体平台
        if (scene.IsSolid(ladderX, y))
        {
            // 找到平台
            TraceLog(LOG_INFO, "Found platform at Y=%d for ladder at X=%d", y, ladderX);

            // 额外检查：这个平台是否在梯子的路径上
            if (scene.IsLadder(ladderX, y))
            {
                // 平台位置也有梯子，继续向下找真正的平台
                TraceLog(LOG_INFO, "Platform at Y=%d also has ladder, continuing search", y);
                continue;
            }

            return y;
        }
    }

    // 没有找到平台，返回地图底部附近
    TraceLog(LOG_WARNING, "No platform found below ladder at X=%d, using Y=%d", ladderX, MAP_HEIGHT - 2);
    return MAP_HEIGHT - 2;
}

void Barrel::Update(GameScene& scene)
{
    float dt = GetFrameTime();
    int tileSize = scene.GetTileSize();

    int tileX = (int)(position.x / tileSize);
    int tileY = (int)((position.y + groundOffset) / tileSize);

    UpdateAnimation();

    switch (state)
    {
    case BarrelState::ROLLING:
    {
        // 水平移动
        position.x += (movingRight ? 1.0f : -1.0f) * speed * dt;  // 使用 1.0f 避免转换警告

        tileX = (int)(position.x / tileSize);
        tileY = (int)((position.y + groundOffset) / tileSize);

        bool onGround = scene.IsSolid(tileX, tileY + 1);
        int nextX = tileX + (movingRight ? 1 : -1);
        bool hasGroundAhead = scene.IsSolid(nextX, tileY + 1);

        // 增强的梯子检测
        bool shouldEnterLadder = false;
        int ladderDetectX = tileX;
        int ladderDetectY = tileY;

        // 1. 检查当前位置下方是否有梯子
        if (scene.IsLadder(tileX, tileY + 1)) {
            shouldEnterLadder = true;
            ladderDetectX = tileX;
            ladderDetectY = tileY + 1;
            TraceLog(LOG_INFO, "Ladder detected below current position at (%d, %d)", ladderDetectX, ladderDetectY);
        }

        // 2. 检查当前位置是否已经在梯子上
        if (!shouldEnterLadder && scene.IsLadder(tileX, tileY)) {
            shouldEnterLadder = true;
            ladderDetectX = tileX;
            ladderDetectY = tileY;
            TraceLog(LOG_INFO, "Already on ladder at (%d, %d)", ladderDetectX, ladderDetectY);
        }

        // 3. 检查前方位置下方是否有梯子
        if (!shouldEnterLadder && scene.IsLadder(nextX, tileY + 1)) {
            shouldEnterLadder = true;
            ladderDetectX = nextX;
            ladderDetectY = tileY + 1;
            TraceLog(LOG_INFO, "Ladder detected ahead at (%d, %d)", ladderDetectX, ladderDetectY);
        }

        // 状态转换
        if (shouldEnterLadder && onGround)
        {
            state = BarrelState::FALLING_DOWN_LADDER;
            currentAnimationFrames = ladderFrames;
            currentFrame = 0;

            currentLadderX = ladderDetectX;
            currentLadderY = ladderDetectY;
            position.x = (float)(currentLadderX * tileSize);  // 显式转换为 float

            // 调整Y位置
            position.y = (float)((currentLadderY - 1) * tileSize + (tileSize - groundOffset));

            ladderProgress = 0.0f;
            targetPlatformY = FindNextPlatformBelow(scene, currentLadderX, currentLadderY);

            TraceLog(LOG_INFO, "Barrel entering ladder at (%d, %d), target platform at y=%d",
                currentLadderX, currentLadderY, targetPlatformY);
        }
        else if (onGround && !hasGroundAhead)
        {
            state = BarrelState::FALLING;
            currentAnimationFrames = fallingFrames;
            currentFrame = 0;
            position.x += (movingRight ? 5.0f : -5.0f);
            position.y += 2.0f;

            TraceLog(LOG_INFO, "Barrel falling from edge at (%d, %d)", tileX, tileY);
        }
        else
        {
            // 贴地逻辑
            for (int y = tileY; y < MAP_HEIGHT; y++)
            {
                if (scene.IsSolid(tileX, y))
                {
                    float groundY = (float)(y * tileSize) + scene.GetVisualOffsetY(tileX, y);
                    if (position.y + groundOffset <= groundY + 6.0f)
                    {
                        position.y = groundY - groundOffset;
                    }
                    break;
                }
            }
        }
        break;
    }

    case BarrelState::FALLING_DOWN_LADDER:
    {
        int tileSize = scene.GetTileSize();

        // 确保使用梯子动画
        if (currentAnimationFrames.empty() || currentAnimationFrames.size() != ladderFrames.size())
        {
            currentAnimationFrames = ladderFrames;
            currentFrame = 0;
        }

        // 检查目标平台是否有效
        if (targetPlatformY < 0 || targetPlatformY >= MAP_HEIGHT)
        {
            targetPlatformY = FindNextPlatformBelow(scene, currentLadderX, currentLadderY);
        }

        // 计算目标位置
        float targetY = (float)(targetPlatformY * tileSize) - groundOffset;
        float currentY = position.y;
        float distanceToTarget = targetY - currentY;

        // 垂直下降
        if (distanceToTarget > 2.0f)
        {
            position.y += ladderRollSpeed * dt;
            ladderProgress += ladderRollSpeed * dt / (float)tileSize;
        }
        else
        {
            // 到达目标平台
            position.y = targetY;

            int groundCheckY = (int)((position.y + groundOffset) / tileSize);
            if (scene.IsSolid(currentLadderX, groundCheckY))
            {
                movingRight = !movingRight;
                state = BarrelState::ROLLING;
                currentAnimationFrames = rollingFrames;
                currentFrame = 0;
                position.x += (movingRight ? 5.0f : -5.0f);

                TraceLog(LOG_INFO, "Barrel reached platform at y=%d", targetPlatformY);
            }
            else
            {
                targetPlatformY = FindNextPlatformBelow(scene, currentLadderX, groundCheckY);
            }
        }

        // 安全限制
        if (position.y > (float)(scene.GetScreenHeight() + 100))
        {
            state = BarrelState::FALLING;
            TraceLog(LOG_WARNING, "Barrel exceeded screen bounds");
        }

        break;
    }

    case BarrelState::FALLING:
    {
        position.y += 220.0f * dt;

        tileX = (int)(position.x / tileSize);
        tileY = (int)((position.y + groundOffset) / tileSize);

        bool groundFound = false;
        for (int y = tileY; y < MAP_HEIGHT; y++)
        {
            if (scene.IsSolid(tileX, y))
            {
                float groundY = (float)(y * tileSize) + scene.GetVisualOffsetY(tileX, y);
                if (position.y + groundOffset >= groundY)
                {
                    position.y = groundY - groundOffset;
                    movingRight = !movingRight;
                    state = BarrelState::ROLLING;
                    currentAnimationFrames = rollingFrames;
                    currentFrame = 0;
                    groundFound = true;
                }
                break;
            }
        }

        if (position.y > (float)(scene.GetScreenHeight() + 100))
        {
            position.y = -1000.0f;
        }
        break;
    }
    }

    // 边界限制
    if (position.x < 0.0f)
    {
        position.x = 0.0f;
        movingRight = true;
    }
    if (position.x > (float)(scene.GetScreenWidth() - GetWidth()))
    {
        position.x = (float)(scene.GetScreenWidth() - GetWidth());
        movingRight = false;
    }
}

void Barrel::Draw()
{
    if (!currentAnimationFrames.empty() && currentFrame < (int)currentAnimationFrames.size())
    {
        DrawTextureEx(
            currentAnimationFrames[currentFrame],
            position,
            0.0f,
            2.5f,
            WHITE
        );
    }
}