#include "Enemy.h"
#include "GameScene.h"
#include "resource_dir.h"

Enemy::Enemy()
    : currentState(EnemyState::BARREL_GRAB),
    currentFrame(0),
    animDirection(1),
    hasBarrel(true),
    isGoingForward(true),
    frameCounter(0.0f),
    frameSpeed(0.5f),
    position({ 95.0f, 130.0f }),
    scale(2.5f),
    currentBarrelType(BarrelType::NORMAL),
    behavior(EnemyBehavior::THROW_BARRELS),
    currentTexture({ 0 })
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

    //ciclo decorativo
    idleTexture = LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Idle1.png");
    emote1Texture = LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Emote1.png");
    emote2Texture = LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Emote2.png");
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
    // (^^)d
    // Si está en modo STATIONARY, no hacer animación de lanzamiento
    if (behavior == EnemyBehavior::STATIONARY) {
        // Animación simple
        frameCounter += GetFrameTime();
        if (frameCounter >= frameSpeed * 2) {  // Más lento
            frameCounter = 0.0f;
            currentFrame = (currentFrame + animDirection);

            int maxFrame = (int)dkWithBarrelTextures.size() - 1;
            if (currentFrame >= maxFrame) {
                currentFrame = maxFrame;
                animDirection = -1;
            }
            if (currentFrame <= 0) {
                currentFrame = 0;
                animDirection = 1;
            }
        }
        return;
    }
    
    /////////////////////////////////////////////////7
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

void Enemy::UpdateDecorativeCycle() {
    frameCounter += GetFrameTime();

    if (frameCounter < 3.0f) {
        // 3 segundos: Idle1
        currentFrame = 0;
        currentTexture = idleTexture;
    }
    else if (frameCounter < 4.0f) {
        // 1 segundo: Emote1
        currentFrame = 1;
        currentTexture = emote1Texture;
    }
    else if (frameCounter < 5.0f) {
        // 1 segundo: Emote2
        currentFrame = 2;
        currentTexture = emote2Texture;
    }
    else {
        // Reiniciar ciclo
        frameCounter = 0.0f;
    }
}

void Enemy::Update(GameScene& scene)
{
    switch (behavior) {
    case EnemyBehavior::THROW_BARRELS:
        // Scene 1: Animación normal + actualizar barriles
        UpdateAnimation();
        for (auto& b : barrels)
            b.Update(scene);
        break;

    case EnemyBehavior::STATIONARY:
        // Scene 2: Solo animación decorativa, sin barriles
        UpdateAnimation();
        // No barriles (no los lanza)
        break;

    case EnemyBehavior::DECORATIVE_CYCLE:
        UpdateDecorativeCycle();  
        break;
    }
}

void Enemy::Draw() {

    if (behavior == EnemyBehavior::DECORATIVE_CYCLE) {
        if (currentTexture.id != 0) {
            DrawTextureEx(currentTexture, position, 0.0f, scale, WHITE);
        }
        return;
    }

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
    
    // (^^)d
    // 不在scene1 不画THROW_BARRELS
    if (behavior == EnemyBehavior::THROW_BARRELS) {
        for (auto& b : barrels)
            b.Draw();
    }
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
