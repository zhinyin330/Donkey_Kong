#include "Enemy.h"
#include "Scene.h"
#include "resource_dir.h"

Enemy::Enemy()
    : currentState(EnemyState::BARREL_GRAB),
    currentFrame(0),
    animDirection(1),
    hasBarrel(true),
    isGoingForward(true),
    frameCounter(0.0f),
    frameSpeed(0.5f),
    position({ 95.0f, 118.0f }),
    scale(2.8f),
    currentBarrelType(BarrelType::NORMAL)
{
    dkWithBarrelTextures = {
        LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_L.png"),
        LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_M.png"),
        LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_R.png")
    };

    dkEmptyTextures = {
        LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_L.png"),
        LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Idle1.png"),
        LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_R.png")
    };
}

Enemy::~Enemy() {
    for (auto& tex : dkWithBarrelTextures) UnloadTexture(tex);
    for (auto& tex : dkEmptyTextures) UnloadTexture(tex);
    // ⭐ 丢桶
}
void Enemy::SpawnBarrel()
{
    Vector2 spawnPos = {
        position.x + 120,
        position.y + 50
    };

    barrels.emplace_back(currentBarrelType, spawnPos);
}

void Enemy::UpdateAnimation() {
    frameCounter += GetFrameTime();

    if (frameCounter >= frameSpeed) {
        frameCounter = 0.0f;

        // --- 逻辑修正：先处理状态切换，再更新帧 ---
        int maxFrame = (int)dkWithBarrelTextures.size() - 1;

        // ⭐ 丢桶时机（中 → 右）
        if (currentFrame == 1 && isGoingForward && animDirection == 1) {
            hasBarrel = false;

            SpawnBarrel();  //  生成桶
        }

        // 到右侧，开始回程
        if (currentFrame == maxFrame && isGoingForward) {
            isGoingForward = false;
            animDirection = -1;
        }
        // 回到左侧，重新拿桶
        else if (currentFrame == 0 && !isGoingForward) {
            isGoingForward = true;
            animDirection = 1;

            hasBarrel = true;

            //  随机桶
            currentBarrelType = (GetRandomValue(0, 1) == 0)
                ? BarrelType::NORMAL
                : BarrelType::BLUE_BARREL;
        }

        currentFrame += animDirection;

        if (currentFrame >= maxFrame) currentFrame = maxFrame;
        if (currentFrame <= 0) currentFrame = 0;
    }
}

void Enemy::Update(Scene& scene)
{
    UpdateAnimation();

    for (auto& b : barrels)
        b.Update(scene);
}



void Enemy::Draw() {
    if (dkWithBarrelTextures.empty()) return;

    const Texture2D& dkTex = hasBarrel
        ? dkWithBarrelTextures[currentFrame]
        : dkEmptyTextures[currentFrame];

    DrawTexturePro(
        dkTex,
        { 0,0,(float)dkTex.width,(float)dkTex.height },
        { position.x, position.y, dkTex.width * scale, dkTex.height * scale },
        { 0,0 },
        0.0f,
        WHITE
    );
    for (auto& b : barrels)
        b.Draw();
}

void Enemy::ChangeState(EnemyState newState) 
{
    if (currentState == newState) return;

    currentState = newState;

    currentFrame = 0;
    frameCounter = 0.0f;
    animDirection = 1;

    hasBarrel = true;
    isGoingForward = true;
}