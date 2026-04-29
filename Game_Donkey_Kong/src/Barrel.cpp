#include "Barrel.h"
#include "GameScene.h"

Barrel::Barrel(BarrelType t, Vector2 pos)
{
    type = t;
    position = pos;

    speed = 40.0f;
    movingRight = true;
    state = BarrelState::ROLLING;

    currentFrame = 0;
    frameCounter = 0.0f;
    frameSpeed = 0.12f;

    groundOffset = 35.0f;

    // =========================
    // 加载动画帧（4帧循环）
    // =========================
    if (type == BarrelType::NORMAL)
    {
        frames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov1.png"));
        frames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov2.png"));
        frames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov3.png"));
        frames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov4.png"));
    }
    else
    {
        frames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov1.png"));
        frames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov2.png"));
        frames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov3.png"));
        frames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov4.png"));
    }
}
void Barrel::UpdateAnimation()
{
    frameCounter += GetFrameTime();

    if (frameCounter >= frameSpeed)
    {
        frameCounter = 0.0f;
        currentFrame = (currentFrame + 1) % frames.size();
    }
}

void Barrel::Update(GameScene& scene)
{
    float dt = GetFrameTime();
    int tileSize = scene.GetTileSize();

    UpdateAnimation();

    // 1水平移动
    if (state == BarrelState::ROLLING)
    {
        position.x += (movingRight ? 1 : -1) * speed * dt;
    }

    // =========================
    // 2当前 tile
    // =========================
    int tileX = (int)((position.x ) / tileSize);
    int tileY = (int)((position.y + groundOffset) / tileSize);

    bool onGround = scene.IsSolid(tileX, tileY + 1);
    // =========================
    // 贴地逻辑
    // =========================

    bool groundFound = false;
    bool justLanded = false;

    for (int y = tileY; y < 22; y++)
    {
        if (scene.IsSolid(tileX, y))
        {
            float groundY = y * tileSize + scene.GetVisualOffsetY(tileX, y);

            if (position.y + groundOffset <= groundY + 6.0f)
            {
                position.y = groundY - groundOffset;

                // ⭐ 如果是从掉落状态落地 → 必须反向
                if (state == BarrelState::FALLING)
                {
                    movingRight = !movingRight;
                }

                state = BarrelState::ROLLING;
                groundFound = true;
            }
            break;
        }
    }

    // =========================
    // 掉落
    // =========================
    if (!groundFound && state == BarrelState::FALLING)
    {
        position.y += 220.0f * dt;
    }


    // =========================
    // 边缘检测（反向 + 掉层）
    // =========================
    int nextX = tileX + (movingRight ? 1 : -1);
    bool hasGroundAhead = scene.IsSolid(nextX, tileY + 1);

    if (state == BarrelState::ROLLING && onGround && !hasGroundAhead)
    {
        state = BarrelState::FALLING;

        // ⭐ 轻微推出边缘，避免卡 tile
        position.x += (movingRight ? 1 : -1) * 5.0f;
        position.y += 2.0f;
    }
}

void Barrel::Draw()
{
    DrawTextureEx(
        frames[currentFrame],
        position,
        0.0f,
        2.5f,
        WHITE
    );
}