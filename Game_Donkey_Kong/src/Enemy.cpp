#include "Enemy.h"
#include "resource_dir.h"

Enemy::Enemy()
    : currentState(EnemyState::BARREL_GRAB),
    currentFrame(0),
    animDirection(1),
    hasBarrel(true),
    isGoingForward(true),
    frameCounter(0.0f),
    frameSpeed(0.5f),
    position({ 100.0f, 118.0f }),
    scale(2.8f)
{
    // 1. 加载拿桶时的大金刚动画帧
    dkWithBarrelTextures.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_L.png"));
    dkWithBarrelTextures.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_M.png"));
    dkWithBarrelTextures.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_R.png"));

    // 2. 加载空手时的大金刚动画帧
    dkEmptyTextures.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_L.png"));
    dkEmptyTextures.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Idle1.png"));
    dkEmptyTextures.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_R.png"));

    // 3. 加载木桶贴图
    barrelSide = LoadTexture("Barrel/Dk_Barrel_Mov1.png");
    barrelFront = LoadTexture("Barrel/Dk_Barrel_Fall1.png");

    // 4. 定义每一帧木桶位置偏移
    barrelOffsets = {
        { -4.0f, 18.0f },   // 帧0 (左侧)
        { 15.0f, 22.0f },   // 帧1 (正面)
        { 45.0f, 18.0f }    // 帧2 (右侧)
    };
}

Enemy::~Enemy() {
    for (auto& tex : dkWithBarrelTextures) UnloadTexture(tex);
    for (auto& tex : dkEmptyTextures) UnloadTexture(tex);
    UnloadTexture(barrelSide);
    UnloadTexture(barrelFront);
}

void Enemy::UpdateAnimation() {
    frameCounter += GetFrameTime();

    if (frameCounter >= frameSpeed) {
        frameCounter = 0.0f;

        // --- 逻辑修正：先处理状态切换，再更新帧 ---
        int maxFrame = (int)dkWithBarrelTextures.size() - 1;

        // 如果已经在最右侧，且状态是去程，说明这一帧已经画完了有桶的右侧身
        if (currentFrame == maxFrame && isGoingForward) {
            // 开始回程
            isGoingForward = false;
            animDirection = -1; // 掉头向左
            hasBarrel = false;  // 这时桶才真正“离开手”
        }
        // 如果已经在最左侧，且状态是回程
        else if (currentFrame == 0 && !isGoingForward) {
            // 开始去程
            isGoingForward = true;
            animDirection = 1;  // 重新向右
            hasBarrel = true;   // 重新抓起桶
        }

        // --- 然后再更新帧 ---
        currentFrame += animDirection;

        // --- 边界锁定 (安全保护) ---
        if (currentFrame >= maxFrame) currentFrame = maxFrame;
        if (currentFrame <= 0) currentFrame = 0;
    }
}

void Enemy::Update() {
    // 只有在抓桶状态下才更新动画
    if (currentState == EnemyState::BARREL_GRAB) {
        UpdateAnimation();
    }
}

void Enemy::Draw() {
    // 基础安全检查：如果贴图没加载成功或索引异常则不绘制
    if (dkWithBarrelTextures.empty() || currentFrame < 0 || currentFrame >= (int)dkWithBarrelTextures.size()) return;

    // --- 1. 选择大金刚贴图 ---
    const Texture2D& dkTex = hasBarrel ? dkWithBarrelTextures[currentFrame] : dkEmptyTextures[currentFrame];

    // 绘制大金刚
    DrawTexturePro(
        dkTex,
        { 0.0f, 0.0f, (float)dkTex.width, (float)dkTex.height },
        { position.x, position.y, dkTex.width * scale, dkTex.height * scale },
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    // --- 2. 绘制木桶 ---
    if (hasBarrel && currentFrame < (int)barrelOffsets.size()) {
        const Texture2D& targetBarrel = (currentFrame == 1) ? barrelFront : barrelSide;

        Vector2 bPos = {
            position.x + barrelOffsets[currentFrame].x * scale,
            position.y + barrelOffsets[currentFrame].y * scale
        };

        DrawTextureEx(targetBarrel, bPos, 0.0f, scale, WHITE);
    }
}

void Enemy::ChangeState(EnemyState newState) {
    if (currentState == newState) return;

    currentState = newState;
    currentFrame = 0;
    frameCounter = 0.0f;
    animDirection = 1;
    hasBarrel = true;
    isGoingForward = true;
}