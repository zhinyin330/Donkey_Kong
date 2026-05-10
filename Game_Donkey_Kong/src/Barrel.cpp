// Barrel.cpp

#include "Barrel.h"
#include "GameScene.h"
#define MAP_HEIGHT 22

Barrel::Barrel(BarrelType t, Vector2 pos)
{
    type = t;
    position = pos;
    speed = 80.0f;
    verticalDropSpeed = 140.0f;

    movingRight = true;
    state = BarrelState::ROLLING;

    currentFrame = 0;
    frameCounter = 0.0f;
    frameSpeed = 0.1f;

    groundOffset = 35.0f;
    // 默认目标平台
    targetPlatformY = -1;

    // 加载动画
    LoadFrames();

   // 设置纵向下落触发点
    // 第一层
    verticalDropTriggers.push_back({ 650.0f,130.0f });
    // 第二层
    verticalDropTriggers.push_back({ 180.0f,260.0f });
    // 第三层
    verticalDropTriggers.push_back({ 620.0f,390.0f });
    // 第四层
    verticalDropTriggers.push_back({ 200.0f,520.0f });
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
    }
    // 默认使用滚动动画
    currentAnimationFrames = rollingFrames;
}

void Barrel::UpdateAnimation()
{
    frameCounter += GetFrameTime();

    if (frameCounter >= frameSpeed)
    {
        frameCounter = 0.0f;

        if (!currentAnimationFrames.empty())
        {
            currentFrame =
                (currentFrame + 1)
                % currentAnimationFrames.size();
        }
    }
}


bool Barrel::CheckVerticalDropTrigger()
{
    // 检测所有触发点
    for (const auto& trigger : verticalDropTriggers)
    {
        // 给一个误差范围
        float xRange = 10.0f;
        float yRange = 10.0f;

        bool insideX =
            position.x >= trigger.x - xRange &&
            position.x <= trigger.x + xRange;

        bool insideY =
            position.y >= trigger.y - yRange &&
            position.y <= trigger.y + yRange;

        // 到达触发点
        if (insideX && insideY)
        {
            return true;
        }
    }

    return false;
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
        // =============================
        // 水平移动
        // =============================
        position.x +=
            (movingRight ? 1.0f : -1.0f)
            * speed
            * dt;

        // =============================
        // 重新计算Tile
        // =============================
        tileX = (int)(position.x / tileSize);

        tileY =
            (int)((position.y + groundOffset) / tileSize);

        // =============================
        // 检测是否到达纵向触发点
        // =============================
        if (CheckVerticalDropTrigger())
        {
            // 切换为纵向下落
            state = BarrelState::VERTICAL_DROP;

            // 切换动画
            currentAnimationFrames = fallingFrames;
            currentFrame = 0;

            // =============================
            // 寻找下一个平台
            // =============================
            for (int y = tileY + 1; y < MAP_HEIGHT; y++)
            {
                if (scene.IsSolid(tileX, y))
                {
                    targetPlatformY = y;
                    break;
                }
            }

            TraceLog(
                LOG_INFO,
                "Vertical drop triggered at %.2f %.2f",
                position.x,
                position.y
            );

            break;
        }

        // =============================
        // 贴地逻辑
        // =============================
        for (int y = tileY; y < MAP_HEIGHT; y++)
        {
            if (scene.IsSolid(tileX, y))
            {
                float groundY =
                    (float)(y * tileSize)
                    + scene.GetVisualOffsetY(tileX, y);

                if (position.y + groundOffset <= groundY + 6.0f)
                {
                    position.y = groundY - groundOffset;
                }

                break;
            }
        }

        break;
    }
        // 指定位置纵向下落
        // 到达平台
    case BarrelState::VERTICAL_DROP:
    {
        // 向下移动
        position.y += verticalDropSpeed * dt;

        // 判断是否到达目标平台
        float targetY =
            (float)(targetPlatformY * tileSize)
            - groundOffset;
        // 到达平台
        if (position.y >= targetY)
        {
            // 对齐平台
            position.y = targetY;
            // 方向反转
            movingRight = !movingRight;
            // 恢复滚动状态
            state = BarrelState::ROLLING;
            // 恢复滚动动画
            currentAnimationFrames = rollingFrames;
            currentFrame = 0;

            TraceLog(LOG_INFO,
                "Barrel reached next platform");
        }

        break;
    }

    // 普通掉落
    case BarrelState::FALLING:
    {
        position.y += 220.0f * dt;
        break;
    }
    }

    // 左边界限制
        if (position.x < 0.0f)
        {
            position.x = 0.0f;
            movingRight = true;
        }

    // 右边界限制
        if (position.x > (float)(scene.GetScreenWidth() - GetWidth()))
        {
            position.x =
                (float)(scene.GetScreenWidth() - GetWidth());

            movingRight = false;
        }
    }

void Barrel::Draw()
{   
    if (!currentAnimationFrames.empty() &&
        currentFrame < (int)currentAnimationFrames.size())
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
void Barrel::Hit()
{
    if (!isHit) {
        isHit = true;
        TraceLog(LOG_INFO, "桶被锤子打中！");
    }
}