#include "Barrel.h"
#include "GameScene.h"

Barrel::Barrel(BarrelType t, Vector2 pos)
{
    type = t;
    position = pos;

    speed = 60.0f;
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
        currentFrame = (currentFrame + 1) % 4;
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
    int tileX = (int)(position.x / tileSize);
    int tileY = (int)(position.y / tileSize);

    // =========================
    // 贴地逻辑
    // =========================
    bool foundGround = false;

    for (int y = tileY; y < 22; y++)
    {
        if (scene.IsSolid(tileX, y))
        {
            float groundY = y * tileSize + scene.GetVisualOffsetY(tileX, y);
            position.y = groundY - groundOffset;
            foundGround = true;
            break;
        }
    }


    // =========================
    // 掉落
    // =========================
    if (!foundGround && state == BarrelState::FALLING)
    {
        position.y += 200.0f * dt;
    }

    // =========================
    // 边缘检测（反向 + 掉层）
    // =========================
    int nextX = movingRight ? tileX + 1 : tileX - 1;

    if (!scene.IsSolid(nextX, tileY))
    {
        // DK经典：掉层 + 反向
        state = BarrelState::FALLING;
        position.y += 2.0f; // 小推进避免卡边缘
        movingRight = !movingRight;
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