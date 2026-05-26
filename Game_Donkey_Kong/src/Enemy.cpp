#include "Enemy.h"
#include "GameScene.h"
#include "resource_dir.h"
#include <algorithm> 

// 初始化静态成员
std::vector<Texture2D> Enemy::normalRollingFrames;
std::vector<Texture2D> Enemy::normalFallingFrames;
std::vector<Texture2D> Enemy::blueRollingFrames;
std::vector<Texture2D> Enemy::blueFallingFrames;
bool Enemy::texturesLoaded = false;
int Enemy::blueBarrelHitCount = 0;

Enemy::Enemy()
    : currentState(EnemyState::BARREL_GRAB),
    currentFrame(0),
    animDirection(1),
    hasBarrel(true),
    isGoingForward(true),
    frameCounter(0.0f),
    frameSpeed(3.0f),
    position({ 95.0f, 130.0f }),
    scale(2.5f),
    currentBarrelType(BarrelType::NORMAL),
    behavior(EnemyBehavior::THROW_BARRELS),
    currentTexture({ 0 }),
    throwTargetPos({ 90, 560 })
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

    // 加载桶的动画贴图
    if (!texturesLoaded) {
        normalRollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Mov1.png"));
        normalFallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Fall1.png"));
        blueRollingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Mov1.png"));
        blueFallingFrames.push_back(LoadTexture("Barrel/Dk_Barrel_Blue_Fall1.png"));
        texturesLoaded = true;
    }
}

Enemy::~Enemy() {
    for (auto& tex : dkWithBarrelTextures) UnloadTexture(tex);
    for (auto& tex : dkEmptyTextures) UnloadTexture(tex);
    UnloadTexture(idleTexture);
    UnloadTexture(emote1Texture);
    UnloadTexture(emote2Texture);
}
void Enemy::SpawnBarrel()
{
    Vector2 spawnPos;
    // ===== 区分投掷模式和普通模式的起始位置 =====
    if (currentBarrelType == BarrelType::BLUE_BARREL && isThrowModeActive)
    {
        // 投掷模式蓝桶：从 DK 手部中心飞出
        spawnPos = { position.x + 55.0f, position.y + 50.0f };
    }
    else
    {
        // 普通桶：原来的位置
        spawnPos = { position.x + 120.0f, position.y + 45.0f };
    }

    Barrel newBarrel(currentBarrelType, spawnPos);

    if (currentBarrelType == BarrelType::BLUE_BARREL && isThrowModeActive)
    {
        newBarrel.SetThrowMode(throwTargetPos);
        TraceLog(LOG_INFO, "Blue barrel THROW from (%.0f, %.0f)", spawnPos.x, spawnPos.y);
    }

    barrels.push_back(newBarrel);
}

void Enemy::UpdateAnimation() {
    // (^^)d
    // Si está en modo STATIONARY, no hacer animación de lanzamiento
    if (behavior == EnemyBehavior::STATIONARY) {
        // Animación simple
        frameCounter += GetFrameTime();
        if (frameCounter >= frameSpeed * 2.0f) {  // Más lento
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

        int maxFrame = (int)dkWithBarrelTextures.size() - 1;

        // 丢桶逻辑
        if (currentFrame == 1 && isGoingForward && animDirection == 1) {
            hasBarrel = false;

            isThrowModeActive = false;

            if (currentBarrelType == BarrelType::BLUE_BARREL)
            {
                if (blueBarrelHitCount < MAX_BLUE_BARREL_HITS)
                {
                    int throwChance = GetRandomValue(0, 100);
                    if (throwChance < 50)
                    {
                        isThrowModeActive = true;
                        throwTargetPos = { 90, 560 };
                        TraceLog(LOG_INFO, "Blue barrel THROW mode activated! Target: (120, 560)");
                    }
                }
            }

            SpawnBarrel();
        }

        // 正常增加帧
        currentFrame += animDirection;

        // 边界检查 - 右侧
        if (currentFrame >= maxFrame) {
            currentFrame = maxFrame;
            if (isGoingForward) {
                isGoingForward = false;
                animDirection = -1;
            }
        }

        // 边界检查 - 左侧
        if (currentFrame <= 0) {
            currentFrame = 0;
            if (!isGoingForward) {
                isGoingForward = true;
                animDirection = 1;
                hasBarrel = true;
                isThrowModeActive = false;

                currentBarrelType = (GetRandomValue(0, 1) == 0)
                    ? BarrelType::NORMAL
                    : BarrelType::BLUE_BARREL;
            }
        }

        // ===== 投掷模式：丢完桶后立即返回 =====
        // 当处于投掷模式、没拿桶、正在向右走时，强制回程
        if (isThrowModeActive && !hasBarrel && isGoingForward) {
            isGoingForward = false;
            animDirection = -1;
            currentFrame = maxFrame;  // 跳到最右帧
            TraceLog(LOG_INFO, "Throw mode: immediate return!");
        }
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
        // 只更新没有被锤子打中的桶（被打中的桶应消失，不再移动）
        for (auto& b : barrels) {
            if (!b.IsHit()) {
                b.Update(scene);
            }
        }
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
    // 检查贴图是否有效
    if (dkWithBarrelTextures.empty() || currentFrame >= (int)dkWithBarrelTextures.size()) {
        return;
    }

    // 选择当前帧的贴图
    const Texture2D& dkTex = hasBarrel
        ? dkWithBarrelTextures[currentFrame]
        : dkEmptyTextures[currentFrame];

    DrawTexturePro(
        dkTex,
        { 0.0f, 0.0f, (float)dkTex.width, (float)dkTex.height },  
        { position.x, position.y, dkTex.width * scale, dkTex.height * scale },
        { 0.0f, 0.0f }, 
        0.0f,
        WHITE
    );

    // ========== DIBUJAR BARRIL EN MANOS (frame M, con barril) ==========
    if (hasBarrel && currentFrame == 1 && behavior == EnemyBehavior::THROW_BARRELS) {
        Texture2D barrelInHands;
        if (currentBarrelType == BarrelType::BLUE_BARREL) {
            barrelInHands = blueFallingFrames[0];
        }
        else {
            barrelInHands = normalFallingFrames[0];
        }

        float barrelScale = 3.0f;
        Vector2 barrelPos = {
            position.x + 37,
            position.y + 38
        };

        DrawTextureEx(barrelInHands, barrelPos, 0.0f, barrelScale, WHITE);
    }
    
    // 不在scene1 不画THROW_BARRELS
    if (behavior == EnemyBehavior::THROW_BARRELS) {
        // 只绘制没有被锤子打中的桶
        for (auto& b : barrels) {
            if (!b.IsHit()) {
                b.Draw();
            }
        }
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

