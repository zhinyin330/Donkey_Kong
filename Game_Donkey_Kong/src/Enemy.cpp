#include "Enemy.h"

Enemy::Enemy() {
    // 加载 Donkey Kong 抓桶动画帧（顺序很重要）
    barrelGrabTextures.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_L.png"));
    barrelGrabTextures.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_M.png"));
    barrelGrabTextures.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_BarrelGrab_R.png"));

    // 初始贴图
    currentTexture = barrelGrabTextures[0];

    // 初始状态
    currentState = EnemyState::BARREL_GRAB;

    // 动画参数
    currentFrame = 0;
    frameCounter = 0.0f;
    frameSpeed = 0.8f;   // 越小越快（可以调）

    // 位置（左上角）
    position = { 100.0f, 144.0f };

    // 缩放
    scale = 2.0f;

    // 朝向
    facingRight = true;
}

Enemy::~Enemy() {
    // 释放所有贴图
    for (Texture2D& tex : barrelGrabTextures) {
        UnloadTexture(tex);
    }
}

void Enemy::ChangeState(EnemyState newState) {
    if (currentState != newState) {
        currentState = newState;

        // 重置动画
        currentFrame = 0;
        frameCounter = 0.0f;

        // 设置当前贴图
        switch (currentState) {
        case EnemyState::BARREL_GRAB:
            currentTexture = barrelGrabTextures[0];
            break;
        }
    }
}

void Enemy::UpdateAnimation() {
    frameCounter += GetFrameTime();

    if (frameCounter >= frameSpeed) {
        frameCounter = 0.0f;
        currentFrame++;

        switch (currentState) {
        case EnemyState::BARREL_GRAB:
            if (currentFrame >= barrelGrabTextures.size()) {
                currentFrame = 0; // 循环播放
            }
            currentTexture = barrelGrabTextures[currentFrame];
            break;
        }
    }
}

void Enemy::Update() {
    // 目前敌人只做动画（没有AI）
    UpdateAnimation();
}

void Enemy::Draw() {
    Rectangle source = {
        0,
        0,
        (float)currentTexture.width,
        (float)currentTexture.height
    };

    Rectangle dest = {
        position.x,
        position.y,
        currentTexture.width * scale,
        currentTexture.height * scale
    };

    Vector2 origin = { 0, 0 };

    // 翻转
    if (!facingRight) {
        source.width = -source.width;
    }

    DrawTexturePro(currentTexture, source, dest, origin, 0.0f, WHITE);

    // （可选）调试框
    DrawRectangleLines(
        position.x,
        position.y,
        currentTexture.width * scale,
        currentTexture.height * scale,
        RED
    );
}