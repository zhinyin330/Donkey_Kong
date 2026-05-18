#include "Barrel.h"
#include "GameScene.h"

#define MAP_HEIGHT 22

 
//  构造函数 初始化所有参数 + 触发点
 
Barrel::Barrel(BarrelType t, Vector2 pos)
{
    type = t;
    position = pos;

    speed = 80.0f;
    verticalDropSpeed = 80.0f;

    movingRight = true;
    state = BarrelState::ROLLING;

    currentFrame = 0;
    frameCounter = 0.0f;
    frameSpeed = 0.1f;

    groundOffset = 29.5f;

    targetPlatformY = -1;
    dropTargetX = 0.0f;

    inDropPhase = false;          //  新增：是否处于下落阶段
    triggerLocked = false;
    triggerLockTimer = 0.0f;

    hasBeenJumped = false;

    LoadFrames();

     
    //  DROP 触发点 进入纵向下落1
    verticalDropTriggers.push_back({ {638.0f, 205.0f, 32.0f, 32.0f},TriggerType::DROP, 0,1.0f });
    verticalDropTriggers.push_back({ {125.0f,305.0f, 32.0f, 32.0f}, TriggerType::DROP, 0,1.0f });
    verticalDropTriggers.push_back({ {638.0f,400.0f, 32.0f, 32.0f}, TriggerType::DROP, 0,1.0f });
    verticalDropTriggers.push_back({ {125.0f,495.0f, 32.0f, 32.0f}, TriggerType::DROP, 0,1.0f });
    verticalDropTriggers.push_back({ {638.0f,595.0f, 32.0f, 32.0f}, TriggerType::DROP, 0,1.0f });

    // RETURN 触发恢复横向移动
    // direction 方向控制2
    verticalDropTriggers.push_back({ {650.0f,295.0f, 32.0f, 32.0f}, TriggerType::RETURN, -1,1.0f });
    verticalDropTriggers.push_back({ {140.0f,390.0f, 32.0f, 32.0f}, TriggerType::RETURN,  1,1.0f });
    verticalDropTriggers.push_back({ {650.0f,490.0f, 32.0f, 32.0f}, TriggerType::RETURN, -1,1.0f });
    verticalDropTriggers.push_back({ {140.0f,585.0f, 32.0f, 32.0f}, TriggerType::RETURN,  1,1.0f });
    verticalDropTriggers.push_back({ {650.0f,675.0f, 32.0f, 32.0f}, TriggerType::RETURN, -1,1.0f });
    
    // direction 方向控制3
    verticalDropTriggers.push_back({ {255.0f,305.0f, 32.0f, 32.0f}, TriggerType::DROP, 0,0.35f });//有几率触发
    verticalDropTriggers.push_back({ {415.0f,400.0f, 32.0f, 32.0f}, TriggerType::DROP, 0,0.15f });//有几率触发
    verticalDropTriggers.push_back({ {353.0f,495.0f, 32.0f, 32.0f}, TriggerType::DROP, 0,0.50f });//有几率触发
    // direction 方向控制4
    verticalDropTriggers.push_back({ {270.0f,390.0f, 32.0f, 32.0f}, TriggerType::RETURN,  1,1.0f });
    verticalDropTriggers.push_back({ {420.0f,490.0f, 32.0f, 32.0f}, TriggerType::RETURN, -1,1.0f });
    verticalDropTriggers.push_back({ {370.0f,585.0f, 32.0f, 32.0f}, TriggerType::RETURN,  1,1.0f });

}

// 加载动画
void Barrel::LoadFrames()
{
    if (type == BarrelType::NORMAL)
    {
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov1.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov2.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov3.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov4.png"));

        fallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Fall1.png"));
        fallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Fall2.png"));
    }
    else
    {
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov1.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov2.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov3.png"));
        rollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov4.png"));

        fallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Fall1.png"));
        fallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Fall2.png"));
    }

    currentAnimationFrames = rollingFrames;
}

//  动画更新 
void Barrel::UpdateAnimation()
{
    frameCounter += GetFrameTime();

    if (frameCounter >= frameSpeed)
    {
        frameCounter = 0.0f;

        if (!currentAnimationFrames.empty())
        {
            currentFrame++;
            currentFrame %= currentAnimationFrames.size();
        }
    }
}

 
//  检测 DROP 触发 只允许ROLLING阶段触发
bool Barrel::CheckVerticalDropTrigger()
{
    if (inDropPhase)
        return false;

    Rectangle barrelRect =
    {
        position.x,
        position.y,
        GetWidth(),
        GetHeight()
    };

    bool touchingAny = false;

    const VerticalDropTrigger* bestTrigger = nullptr;

    for (const auto& trigger : verticalDropTriggers)
    {
        if (trigger.type != TriggerType::DROP)
            continue;

        if (!CheckCollisionRecs(barrelRect, trigger.rect))
            continue;

        touchingAny = true;

        // 已经尝试过当前区域避免重复抽
        if (hasCheckedTrigger)
            continue;

        hasCheckedTrigger = true;

        // 100%必触发
        if (trigger.triggerChance >= 1.0f)
        {
            bestTrigger = &trigger;
            break;
        }

        // 概率触发
        float roll =
            (float)GetRandomValue(0, 9999) / 10000.0f;

        if (roll <= trigger.triggerChance)
        {
            bestTrigger = &trigger;
            break;
        }

        // 失败只影响当前，不锁全局
        return false;
    }

    // 如果找到了 trigger
    if (bestTrigger != nullptr)
    {
        currentDropTrigger = bestTrigger;
        dropTargetX = bestTrigger->rect.x;
        return true;
    }

    // ⭐离开区域才重置
    if (!touchingAny)
    {
        hasCheckedTrigger = false;
    }

    return false;
}
//  主更新逻辑 
 
void Barrel::Update(GameScene& scene)
{
    float dt = GetFrameTime();
    int tileSize = scene.GetTileSize();

    int tileX = (int)(position.x / tileSize);
    int tileY = (int)((position.y + groundOffset) / tileSize);

    UpdateAnimation();

    //  状态锁：防止瞬间重复触发
    if (triggerLocked)
    {
        triggerLockTimer -= dt;
        if (triggerLockTimer <= 0.0f)
            triggerLocked = false;
    }

    //  ROLLING 状态 
    if (state == BarrelState::ROLLING)
    {
        // 水平移动
        position.x += (movingRight ? 1 : -1) * speed * dt;

        tileX = (int)(position.x / tileSize);
        tileY = (int)((position.y + groundOffset) / tileSize);

        // DROP 触发（核心）
        if (!inDropPhase && CheckVerticalDropTrigger())
        {
            state = BarrelState::VERTICAL_DROP;
            currentAnimationFrames = fallingFrames;
            currentFrame = 0;

            // 对齐到DROP触发点
            if (currentDropTrigger)
            {
                position.x = currentDropTrigger->rect.x;
            }

            inDropPhase = true; //  进入下落阶段
            targetPlatformY = -1;
            // 找落点
            for (int y = tileY + 1; y < MAP_HEIGHT; y++)
            {
                if (scene.IsSolid(tileX, y))
                {
                    targetPlatformY = y;
                    break;
                }
            }

            TraceLog(LOG_INFO, "DROP trigger");
        }

        // 贴地（只在ROLLING）
        for (int y = tileY; y < MAP_HEIGHT; y++)
        {
            if (scene.IsSolid(tileX, y))
            {
                float groundY =
                    (float)(y * tileSize)
                    + scene.GetVisualOffsetY(tileX, y);

                if (position.y + groundOffset <= groundY + 6.0f)
                    position.y = groundY - groundOffset;

                break;
            }
        }
    }
    //  VERTICAL_DROP 状态 
    else if (state == BarrelState::VERTICAL_DROP)
    {
        // 锁定X（平滑吸附）
        position.x += (dropTargetX - position.x) * 10.0f * dt;

        // 下落
        position.y += verticalDropSpeed * dt;

        // RETURN 触发（必须在下落阶段）
        if (inDropPhase && !triggerLocked)
        {
            for (const auto& trigger : verticalDropTriggers)
            {
                if (trigger.type != TriggerType::RETURN)
                    continue;

                Rectangle barrelRect =
                {
                    position.x,
                    position.y,
                    GetWidth(),
                    GetHeight()
                };

                if (CheckCollisionRecs(barrelRect, trigger.rect))
                {
                    movingRight = (trigger.direction == 1);

                    state = BarrelState::ROLLING;
                    currentAnimationFrames = rollingFrames;
                    currentFrame = 0;

                    inDropPhase = false;

                    triggerLocked = true;
                    triggerLockTimer = 0.6f;

                    TraceLog(LOG_INFO, "RETURN trigger");
                    return;
                }
            }
        }


        if (targetPlatformY != -1)
        {
            float targetY =
                (float)(targetPlatformY * tileSize)
                - groundOffset;

            if (position.y >= targetY)
            {
                position.y = targetY;

                state = BarrelState::ROLLING;
                movingRight = !movingRight;

                currentAnimationFrames = rollingFrames;
                currentFrame = 0;

                inDropPhase = false;
            }
        }
    }

}

//  绘制 
void Barrel::Draw()
{
    if (!currentAnimationFrames.empty() &&
        currentFrame < currentAnimationFrames.size())
    {
        DrawTextureEx(
            currentAnimationFrames[currentFrame],
            position,
            0.0f,
            2.2f,
            WHITE
        );
    }
}

//  被攻击 
void Barrel::Hit()
{
    if (!isHit)
    {
        isHit = true;
        TraceLog(LOG_INFO, "桶被锤子打中！");
    }
}