#include "Player.h"
#include "GameScene.h"  
#include "resource_dir.h"
#include <string> 

//Para usarlo correctamente; Duracion del modo immune
const float Player::starModeDuration = 10.0f;
int Player::lives = 3;

Player::Player() {
    bonusSound = LoadSound("audio/bonus.mp3");
    if (bonusSound.frameCount != 0) {
        SetSoundVolume(bonusSound, 0.6f);  // 音量60%
        bonusSoundLoaded = true;
    }
    else {
        bonusSoundLoaded = false;
        TraceLog(LOG_WARNING, "Failed to load bonus sound");
    }

    // Cargar texturas
    idleTexture = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png");
    jumpTexture = LoadTexture("Characters/Mario/Dk_Mario_Jump.png");
    jumpSound = LoadSound("audio/SFXjump.mp3");
    SetSoundVolume(jumpSound, 5.0f);

    // Pasos
    walkSound = LoadSound("audio/Walking.mp3");
    SetSoundVolume(walkSound, 5.0f);

    stepTimer = 0.0f;
    stepInterval = 0.20f;  
    climbStepInterval = 0.4f;//梯子声音间隔

    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk1.png"));
    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk2.png"));
    walkEndTexture = LoadTexture("Characters/Mario/Dk_Mario_WalkEnd.png");

    climbTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Ladder1.png"));
    climbTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Ladder2.png"));

    climbEndTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_LadderEnd1.png"));
    climbEndTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_LadderEnd2.png"));

    deathTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Death1.png"));
    deathTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Death2.png"));
    deathTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Death3.png"));
    deathTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Death4.png"));
    deathEndTexture = LoadTexture("Characters/Mario/Dk_Mario_DeathEnd.png");

    //加载锤子动画
    hammerSwingTextures.push_back(LoadTexture("Items/Dk_Hammer_Up.png"));
    hammerSwingTextures.push_back(LoadTexture("Items/Dk_Hammer_Right.png"));

    starIconTexture = LoadTexture("Items/New_Dk_star1.png");
    hammerIconTexture = LoadTexture("Items/Dk_Hammer_Up.png");

    // 初始化变量
    hasHammer = false;
    isSwingingHammer = false;
    swingHammerTimer = 0.0f;
    swingHammerDuration = 0.3f;
    swingFrame = 0;
    hammerOffset = { 25, 0 };  // 锤子在玩家右边

    // Variables
    currentTexture = idleTexture;
    speed = 3.3f;
    velocityY = 0.0f;
    gravity = 0.13f;
    isJumping = false;
    scale = 2.3f;
    moveX = 0.0f;
    velocityX = 0.0f;
    onLadder = false;
    isClimbing = false;
    climbSpeed = 1.8f;
    moveY = 0.0f;
    exitingLadder = false;
    wasInAir = false;

    isDying = false;
    deathTimer = 0.0f;
    deathFrame = 0;

    // Configuración de hitbox
    baseHitboxOffsetY = 2;
    baseHitboxHeight = 14;

    int feetOffset = baseHitboxOffsetY + baseHitboxHeight;
    jumpHitboxOffsetY = 0;
    jumpHitboxHeight = feetOffset - jumpHitboxOffsetY;

    int tileSize = 32;
    int startTileX = 5;
    int startTileY = 21;
    int platformOffsetY = 8;

    position.x = (float)(startTileX * tileSize);
    position.y = (float)(startTileY * tileSize) + platformOffsetY
        - (baseHitboxOffsetY + baseHitboxHeight) * scale;

    // Animación
    currentState = PlayerState::IDLE;
    currentFrame = 0;
    walkEndCounter = 0;
    frameCounter = 0;
    frameSpeed = 0.15f;
    facingRight = true;
    wasMoving = false;
    isStepPlaying = false;
    wasMovingLeft = false;
    wasMovingRight = false;

    // Star
    starCount = 0;
    //score
    score = 0;

    // Modo estrella
    starMode = false;
    starModeTimer = 0.0f;
    currentTint = WHITE;

    // 锤子系统初始化
    hasHammer = false;
    isSwingingHammer = false;
    swingHammerTimer = 0.0f;
    swingHammerDuration = 0.3f;
    swingFrame = 0;


    jumpedBarrels.clear();

    hammerCooldownTimer = 0.0f;
    hammerCooldownDuration = 5.0f;  // 5秒冷却
    isHammerOnCooldown = false;

    // 加载死亡音效
    deathSound = LoadSound("audio/Dead.mp3");  // 或 .mp3
    deathSoundLoaded = deathSound.frameCount != 0;
    if (deathSoundLoaded) {
        SetSoundVolume(deathSound, 0.8f);
        TraceLog(LOG_INFO, "Player death sound loaded!");
    }
    else {
        TraceLog(LOG_WARNING, "Failed to load player death sound!");
    }

    // 新增：移动平台变量初始化 
    isOnMovingPlatform = false;
    lastMovingPlatformPos = { 0, 0 };
}

Player::~Player() {
    UnloadTexture(idleTexture);
    UnloadTexture(jumpTexture);
    UnloadSound(walkSound);
    UnloadTexture(walkEndTexture);
    for (Texture2D& tex : walkTextures) UnloadTexture(tex);
    for (Texture2D& tex : climbTextures) UnloadTexture(tex);
    for (Texture2D& tex : climbEndTextures) UnloadTexture(tex);
    UnloadSound(jumpSound);
    for (Texture2D& tex : hammerSwingTextures) UnloadTexture(tex);
    for (Texture2D& tex : deathTextures) UnloadTexture(tex);
    UnloadTexture(deathEndTexture);
    UnloadTexture(starIconTexture);
    UnloadTexture(hammerIconTexture);
    // 卸载死亡音效
    if (deathSoundLoaded) {
        UnloadSound(deathSound);
    }
    if (bonusSoundLoaded) {
        UnloadSound(bonusSound);
        bonusSoundLoaded = false;
    }
}

void Player::HandleInput(GameScene& scene) {
    if (isDying) return;
    if (IsKeyPressed(KEY_J) && hasHammer && !isSwingingHammer && currentState != PlayerState::HAMMER_SWING) {
        StartSwingHammer();
        return;
    }

    //  如果正在挥锤，不处理移动、跳跃等其他输入
    if (isSwingingHammer || currentState == PlayerState::HAMMER_SWING) {
        return;
    }

    moveX = 0;
    isClimbing = false;
    moveY = 0;

    int offsetY = baseHitboxOffsetY;
    int height = baseHitboxHeight;

    int centerX = (int)(position.x + currentTexture.width * scale / 2);
    int tileX = centerX / 32;
    int feetY = (int)(position.y + (offsetY + height) * scale);
    int tileY = feetY / 32;

    // Solo recalcular si NO está en CLIMBING
    if (currentState != PlayerState::CLIMBING) {
        int ladderHitbox = scene.GetLadderHitbox(tileX, tileY);
        onLadder = false;

        if (ladderHitbox == 1) {
            onLadder = true;
        }
        else if (ladderHitbox == 2) {
            float localY = position.y + (offsetY + height) * scale - (tileY * 32);
            if (localY >= 16) {
                onLadder = true;
            }
        }
        else if (ladderHitbox == 3) {
            float localY = position.y + (offsetY + height) * scale - (tileY * 32);
            if (localY < 16) {
                onLadder = true;
            }
        }

        if (!onLadder) {
            int bodyY = (int)(position.y + (offsetY + height / 2) * scale);
            int bodyTileY = bodyY / 32;
            int bodyHitbox = scene.GetLadderHitbox(tileX, bodyTileY);

            if (bodyHitbox == 1) {
                onLadder = true;
            }
            else if (bodyHitbox == 2) {
                float localY = position.y + (offsetY + height / 2) * scale - (bodyTileY * 32);
                if (localY >= 16) {
                    onLadder = true;
                }
            }
            else if (bodyHitbox == 3) {
                float localY = position.y + (offsetY + height / 2) * scale - (bodyTileY * 32);
                if (localY < 16) {
                    onLadder = true;
                }
            }
        }
    }

    // Entrar en modo escalera
    if (onLadder && !exitingLadder) {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            isClimbing = true;
            moveY = -1;
            ChangeState(PlayerState::CLIMBING);
        }
        else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            isClimbing = true;
            moveY = 1;
            ChangeState(PlayerState::CLIMBING);
        }
        else if (currentState == PlayerState::CLIMBING) {
            isClimbing = false;
            moveY = 0;
        }
        else {
            onLadder = false;
        }

        // Movimiento horizontal para salir
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            moveX = -1;
            facingRight = false;
            if (currentState == PlayerState::CLIMBING) {
                ChangeState(PlayerState::IDLE);
            }
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            moveX = 1;
            facingRight = true;
            if (currentState == PlayerState::CLIMBING) {
                ChangeState(PlayerState::IDLE);
            }
        }
    }
    

    // Movimiento normal
    if (currentState != PlayerState::CLIMBING) {
        if (IsKeyPressed(KEY_SPACE) && !isJumping) {
            velocityY = -4.0f;
            isJumping = true;
            PlaySound(jumpSound);
        }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            moveX = -1;
            facingRight = false;
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            moveX = 1;
            facingRight = true;
        }
        if (IsKeyPressed(KEY_K) && !starMode && starCount >= maxStars) {
            ActivateStarMode();
        }
    }
  

    UpdateStarMode();

    // Determinar estado
    if (exitingLadder) {
        // Mantener CLIMBING_END
    }
    else if (isJumping) {
        ChangeState(PlayerState::JUMPING);
    }
    else if (currentState == PlayerState::CLIMBING) {
        // Mantener CLIMBING
    }
    else if (moveX != 0) {
        ChangeState(PlayerState::WALKING);
    }
    else {
        if (currentState == PlayerState::WALKING) {
            ChangeState(PlayerState::WALK_END);
        }
        else if (currentState != PlayerState::WALK_END) {
            ChangeState(PlayerState::IDLE);
        }
    }
}

int Player::GetCurrentHitboxOffsetY() {
    if (currentState == PlayerState::JUMPING) {
        return jumpHitboxOffsetY;
    }
    return baseHitboxOffsetY;
}

int Player::GetCurrentHitboxHeight() {
    if (currentState == PlayerState::JUMPING) {
        return jumpHitboxHeight;
    }
    return baseHitboxHeight;
}

void Player::ChangeState(PlayerState newState) {
    if (currentState != newState) {
        float feetY = GetFeetPosition();
        currentState = newState;
        currentFrame = 0;
        frameCounter = 0;
        walkEndCounter = 0;

        switch (currentState) {
        case PlayerState::IDLE:
            currentTexture = idleTexture;
            exitingLadder = false;
            break;
        case PlayerState::WALKING:
            if (!walkTextures.empty()) currentTexture = walkTextures[0];
            exitingLadder = false;
            break;
        case PlayerState::WALK_END:
            currentTexture = walkEndTexture;
            exitingLadder = false;
            break;
        case PlayerState::JUMPING:
            currentTexture = jumpTexture;
            exitingLadder = false;
            break;
        case PlayerState::CLIMBING:
            if (!climbTextures.empty()) currentTexture = climbTextures[0];
            exitingLadder = false;
            break;
        case PlayerState::CLIMBING_END:
            if (!climbEndTextures.empty()) {
                currentTexture = climbEndTextures[0];
                exitingLadder = true;
                currentFrame = 0;
                frameCounter = 0;
                walkEndCounter = 0;
            }
            break;
            // 设置挥锤的第一帧动画
        case PlayerState::HAMMER_SWING:
            exitingLadder = false;
            break;
        }

        if (!isJumping || currentState == PlayerState::JUMPING) {
            SetFeetPosition(feetY);
        }
    }
}

void Player::UpdateAnimation() {

    // 如果正在挥锤，不更新普通动画
    if (currentState == PlayerState::HAMMER_SWING) {
        return;
    }
    frameCounter += GetFrameTime();

    if (frameCounter >= frameSpeed) {
        frameCounter = 0;

        switch (currentState) {
        case PlayerState::IDLE:
            break;

        case PlayerState::WALKING:
            currentFrame++;
            if (currentFrame >= walkTextures.size()) currentFrame = 0;
            currentTexture = walkTextures[currentFrame];
            break;

        case PlayerState::WALK_END:
            walkEndCounter++;
            if (walkEndCounter >= 180) ChangeState(PlayerState::IDLE);
            break;

        case PlayerState::JUMPING:
            break;

        case PlayerState::CLIMBING:
            if (isClimbing) {
                currentFrame++;
                if (currentFrame >= climbTextures.size()) currentFrame = 0;
                currentTexture = climbTextures[currentFrame];
            }
            break;

        case PlayerState::CLIMBING_END:
            currentFrame++;
            if (currentFrame >= climbEndTextures.size()) {
                ChangeState(PlayerState::IDLE);
                onLadder = false;
                isClimbing = false;
                exitingLadder = false;
            }
            else {
                currentTexture = climbEndTextures[currentFrame];
            }
            break;
        }
    }
}

float Player::GetFeetPosition() {
    int currentOffsetY = GetCurrentHitboxOffsetY();
    int currentHeight = GetCurrentHitboxHeight();
    return position.y + (currentOffsetY + currentHeight) * scale;
}

void Player::SetFeetPosition(float feetY) {
    int currentOffsetY = GetCurrentHitboxOffsetY();
    int currentHeight = GetCurrentHitboxHeight();
    position.y = feetY - (currentOffsetY + currentHeight) * scale;
}

void Player::Update(GameScene& scene) {
    // ========== 更新锤子冷却 ==========
    if (isHammerOnCooldown) {
        hammerCooldownTimer -= GetFrameTime();
        if (hammerCooldownTimer <= 0.0f) {
            isHammerOnCooldown = false;
            hammerCooldownTimer = 0.0f;
        }
    }

    // 移动平台跟随逻辑 
    // 如果站在移动平台上，根据平台的移动来更新玩家位置
    if (isOnMovingPlatform && scene.HasMovingPlatforms()) {
        float newPlatY;
        bool hasButton = false;
        bool buttonCollected = false;
        if (scene.CheckMovingPlatformCollision(GetHitbox(), newPlatY, hasButton, buttonCollected) >= 0) {
            // 仍然站在平台上，更新位置跟随平台移动
            // 注意：平台碰撞检测会处理垂直位置，水平跟随需要额外处理
            Vector2 platformPos = scene.GetNearestMovingPlatformPosition(GetHitbox());
            if (platformPos.x != 0 || platformPos.y != 0) {
                // 水平跟随：计算平台移动的差值
                float deltaX = platformPos.x - lastMovingPlatformPos.x;
                if (fabs(deltaX) < 50.0f) {  // 限制最大移动距离，防止异常
                    position.x += deltaX;
                }
                lastMovingPlatformPos = platformPos;
            }
        }
        else {
            // 不再站在平台上
            isOnMovingPlatform = false;
        }
    }

    if (justLandedTimer > 0.0f)
        justLandedTimer -= GetFrameTime();

    UpdateHammerSwing(GetFrameTime());// 先更新挥锤动画
    UpdateAnimation();
    UpdateStarMode();

    UpdateAnimation();
    UpdateStarMode();
    UpdateFloatingTexts();  // 更新浮动文字

    // ================= SISTEMA DE PASOS =================
   
    bool isMovingOnGround = (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT));
    bool isMovingOnLadder = (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN));

    // 在地上移动
    if (isMovingOnGround && !isJumping && !onLadder)
    {
        stepTimer += GetFrameTime();
        if (stepTimer >= stepInterval)
        {
            PlaySound(walkSound);
            stepTimer = 0.0f;
        }
    }
    // 在梯子上移动
    else if (isMovingOnLadder && onLadder)
    {
        stepTimer += GetFrameTime();
        if (stepTimer >= climbStepInterval)  // 使用更慢的间隔
        {
            PlaySound(walkSound);
            stepTimer = 0.0f;
        }
    }
    else
    {
        stepTimer = stepInterval;
    }

    // Si está en animación de salida, solo animación
    if (currentState == PlayerState::CLIMBING_END || currentState == PlayerState::HAMMER_SWING) {
        velocityY = 0;
        isJumping = false;
        return;
    }

    // Si está escalando activamente
    if (currentState == PlayerState::CLIMBING && !exitingLadder) {
        if (isClimbing) {
            position.y += moveY * climbSpeed;
        }
        position.x += moveX * speed;

        // Límites - USAR GameScene::GetScreenWidth/Height
        if (position.x < 0) position.x = 0;
        if (position.x + currentTexture.width * scale > GameScene::GetScreenWidth()) {
            position.x = GameScene::GetScreenWidth() - currentTexture.width * scale;
        }
        if (position.y < 0) position.y = 0;
        if (position.y + currentTexture.height * scale > GameScene::GetScreenHeight()) {
            position.y = GameScene::GetScreenHeight() - currentTexture.height * scale;
        }

        velocityY = 0;
        isJumping = false;

        int centerX = (int)(position.x + currentTexture.width * scale / 2);
        int tileX = centerX / 32;
        int feetY = (int)(position.y + (baseHitboxOffsetY + baseHitboxHeight) * scale);
        int tileY = feetY / 32;

        int currentHitbox = scene.GetLadderHitbox(tileX, tileY);
        int bodyHitbox = scene.GetLadderHitbox(tileX, tileY - 1);

        bool canClimbHere = false;

        if (currentHitbox == 1) {
            canClimbHere = true;
        }
        else if (currentHitbox == 2) {
            float localY = position.y + (baseHitboxOffsetY + baseHitboxHeight) * scale - (tileY * 32);
            if (localY >= 16) canClimbHere = true;
        }
        else if (currentHitbox == 3) {
            float localY = position.y + (baseHitboxOffsetY + baseHitboxHeight) * scale - (tileY * 32);
            if (localY < 16) canClimbHere = true;
        }

        if (!canClimbHere && bodyHitbox > 0) {
            if (bodyHitbox == 1) {
                canClimbHere = true;
            }
            else if (bodyHitbox == 2) {
                float localY = position.y + (baseHitboxOffsetY + baseHitboxHeight / 2) * scale - ((tileY - 1) * 32);
                if (localY >= 16) canClimbHere = true;
            }
            else if (bodyHitbox == 3) {
                float localY = position.y + (baseHitboxOffsetY + baseHitboxHeight / 2) * scale - ((tileY - 1) * 32);
                if (localY < 16) canClimbHere = true;
            }
        }

        if (!canClimbHere) {
            ChangeState(PlayerState::CLIMBING_END);
            isClimbing = false;
            moveY = 0;
            return;
        }

        return;
    }

    // ========== FÍSICA NORMAL ==========

    int tileSize = scene.GetTileSize();
    int currentOffsetY = GetCurrentHitboxOffsetY();
    int currentHeight = GetCurrentHitboxHeight();

    int platformHitboxHeight = scene.GetPlatformHitboxHeight();
    int platformHitboxOffsetY = scene.GetPlatformHitboxOffsetY();

    // ========== HORIZONTAL ==========
    float nextX = position.x + moveX * speed;

    int mapWidthPixels = GameScene::GetScreenWidth();
    if (nextX < 0) nextX = 0;
    if (nextX + currentTexture.width * scale > mapWidthPixels) {
        nextX = mapWidthPixels - currentTexture.width * scale;
    }

    int leftTile = (int)(nextX / tileSize);
    int rightTile = (int)((nextX + currentTexture.width * scale - 1) / tileSize);

    float hitboxTopY = position.y + currentOffsetY * scale;
    float hitboxBottomY = position.y + currentOffsetY * scale + currentHeight * scale;

    int topTile = (int)(hitboxTopY / tileSize);
    int bottomTile = (int)((hitboxBottomY - 1) / tileSize);

    bool horizontalCollision = false;

    int currentLeftTile = (int)(position.x / tileSize);
    int currentRightTile = (int)((position.x + currentTexture.width * scale - 1) / tileSize);

    float feetLevel = hitboxBottomY;
    float headLevel = hitboxTopY;

    float horizontalDirection = moveX;
    if (horizontalDirection == 0) {
        horizontalDirection = (velocityX != 0) ? (velocityX > 0 ? 1.0f : -1.0f) : 0;
    }

    if (horizontalDirection < 0) {
        for (int ty = topTile; ty <= bottomTile + 1 && !horizontalCollision; ty++) {
            if (scene.IsSolid(leftTile, ty)) {
                int tileOffsetY = scene.GetVisualOffsetY(leftTile, ty);
                float platformTop = ty * tileSize + tileOffsetY;
                float platformBottom = platformTop + platformHitboxHeight;

                if (platformTop < headLevel + 8 && platformBottom > feetLevel - 8) {
                    if (platformTop <= feetLevel + 4 && horizontalDirection < 0) {
                        continue;
                    }
                    nextX = (float)(leftTile + 1) * tileSize;
                    horizontalCollision = true;
                }
            }

            if (!horizontalCollision && scene.IsSolid(currentLeftTile, ty)) {
                int tileOffsetY = scene.GetVisualOffsetY(currentLeftTile, ty);
                float platformTop = ty * tileSize + tileOffsetY;
                float platformBottom = platformTop + platformHitboxHeight;

                if (platformTop < headLevel + 8 && platformBottom > feetLevel - 8) {
                    if (platformTop > feetLevel - 4) {
                        nextX = (float)(currentLeftTile + 1) * tileSize;
                        horizontalCollision = true;
                        break;
                    }
                }
            }
        }
    }
    else if (horizontalDirection > 0) {
        for (int ty = topTile; ty <= bottomTile + 1 && !horizontalCollision; ty++) {
            if (scene.IsSolid(rightTile, ty)) {
                int tileOffsetY = scene.GetVisualOffsetY(rightTile, ty);
                float platformTop = ty * tileSize + tileOffsetY;
                float platformBottom = platformTop + platformHitboxHeight;

                if (platformTop < headLevel + 8 && platformBottom > feetLevel - 8) {
                    if (platformTop <= feetLevel + 4 && horizontalDirection > 0) {
                        continue;
                    }
                    nextX = (float)(rightTile * tileSize) - currentTexture.width * scale;
                    horizontalCollision = true;
                }
            }

            if (!horizontalCollision && scene.IsSolid(currentRightTile, ty)) {
                int tileOffsetY = scene.GetVisualOffsetY(currentRightTile, ty);
                float platformTop = ty * tileSize + tileOffsetY;
                float platformBottom = platformTop + platformHitboxHeight;

                if (platformTop < headLevel + 8 && platformBottom > feetLevel - 8) {
                    if (platformTop > feetLevel - 4) {
                        nextX = (float)(currentRightTile * tileSize) - currentTexture.width * scale;
                        horizontalCollision = true;
                        break;
                    }
                }
            }
        }
    }

    // Colisión lateral sin movimiento
    if (!horizontalCollision && moveX == 0) {
        for (int ty = topTile; ty <= bottomTile; ty++) {
            if (scene.IsSolid(currentLeftTile, ty) || scene.IsSolid(currentRightTile, ty)) {
                int tileX = scene.IsSolid(currentLeftTile, ty) ? currentLeftTile : currentRightTile;
                int tileOffsetY = scene.GetVisualOffsetY(tileX, ty);
                float platformTop = ty * tileSize + tileOffsetY;
                float platformBottom = platformTop + platformHitboxHeight;

                if (feetLevel < platformBottom && headLevel > platformTop) {
                    if (tileX == currentLeftTile) {
                        nextX = (float)(currentLeftTile + 1) * tileSize;
                    }
                    else {
                        nextX = (float)(currentRightTile * tileSize) - currentTexture.width * scale;
                    }
                    horizontalCollision = true;
                    break;
                }
            }
        }
    }

    // Pared lateral agresiva
    if (!horizontalCollision) {
        int mapWidthTiles = GameScene::GetScreenWidth() / tileSize;
        int mapHeightTiles = GameScene::GetScreenHeight() / tileSize;

        for (int ty = topTile; ty <= bottomTile; ty++) {
            if (ty < 0 || ty >= mapHeightTiles) continue;

            // Borde izquierdo
            int leftCheckX = currentLeftTile - 1;
            if (leftCheckX >= 0) {
                if (scene.IsSolid(leftCheckX, ty)) {
                    int tileOffsetY = scene.GetVisualOffsetY(leftCheckX, ty);
                    float platformTop = ty * tileSize + tileOffsetY;
                    float platformBottom = platformTop + platformHitboxHeight;

                    if (hitboxBottomY > platformTop + 2 && hitboxTopY < platformBottom - 2) {
                        float tileRight = (leftCheckX + 1) * tileSize;

                        if (position.x < tileRight && position.x + currentTexture.width * scale > leftCheckX * tileSize) {
                            nextX = tileRight;
                            horizontalCollision = true;
                            break;
                        }
                    }
                }
            }

            // Borde derecho
            int rightCheckX = currentRightTile + 1;
            if (!horizontalCollision && rightCheckX < mapWidthTiles) {
                if (scene.IsSolid(rightCheckX, ty)) {
                    int tileOffsetY = scene.GetVisualOffsetY(rightCheckX, ty);
                    float platformTop = ty * tileSize + tileOffsetY;
                    float platformBottom = platformTop + platformHitboxHeight;

                    if (hitboxBottomY > platformTop + 2 && hitboxTopY < platformBottom - 2) {
                        float tileLeft = rightCheckX * tileSize;
                        float marioRight = position.x + currentTexture.width * scale;

                        if (marioRight > tileLeft && position.x < (rightCheckX + 1) * tileSize) {
                            nextX = tileLeft - currentTexture.width * scale;
                            horizontalCollision = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    // ========== NUEVO: Colisión lateral con plataformas manuales (Scene2) ==========
    if (!horizontalCollision && scene.HasNewPlatforms()) {
        float newPlatY;

        // Solo comprobar colisión lateral si Mario está a la altura de las plataformas
        for (auto& plat : scene.GetNewPlatforms()) {
            // ¿Está Mario a la altura de esta plataforma?
            float playerTop = position.y + currentOffsetY * scale;
            float playerBottom = position.y + (currentOffsetY + currentHeight) * scale;
            float platTop = plat.y;
            float platBottom = plat.y + plat.height;

            if (playerBottom > platTop + 4 && playerTop < platBottom - 4) {
                // Colisión por la izquierda de la plataforma
                if (nextX + currentTexture.width * scale > plat.x &&
                    position.x + currentTexture.width * scale <= plat.x + 4) {
                    nextX = plat.x - currentTexture.width * scale;
                    horizontalCollision = true;
                    break;
                }
                // Colisión por la derecha de la plataforma
                if (nextX < plat.x + plat.width &&
                    position.x >= plat.x + plat.width - 4) {
                    nextX = plat.x + plat.width;
                    horizontalCollision = true;
                    break;
                }
            }
        }
    }

    position.x = nextX;

    // ========== VERTICAL ==========
    velocityY += gravity;
    float nextY = position.y + velocityY;

    float nextFeetY = nextY + (currentOffsetY + currentHeight) * scale;
    float nextHeadY = nextY + currentOffsetY * scale;

    int nextFeetTileY = (int)((nextFeetY - 1) / tileSize);
    int nextHeadTileY = (int)((nextHeadY) / tileSize);

    leftTile = (int)(position.x / tileSize);
    rightTile = (int)((position.x + currentTexture.width * scale - 1) / tileSize);

    // Límites verticales
    int mapHeightPixels = GameScene::GetScreenHeight();
    if (nextFeetY > mapHeightPixels) {
        LoseLife();
        StartDeath();
        position.x = 2 * tileSize;
        position.y = 21 * tileSize + 16 - (baseHitboxOffsetY + baseHitboxHeight) * scale;
        velocityY = 0;
        isJumping = false;
        ChangeState(PlayerState::IDLE);
        return;
    }

    if (nextHeadY < 0) {
        position.y = -currentOffsetY * scale;
        velocityY = 0;
    }

    // Colisión al CAER
    if (velocityY >= 0) {
        bool landed = false;
        float groundY = nextY;

        int mapWidthTiles = GameScene::GetScreenWidth() / tileSize;

        int checkLeftTile = leftTile;
        int checkRightTile = rightTile;

        if (scene.CheckAdjacentTiles()) {
            checkLeftTile = leftTile - 1;
            checkRightTile = rightTile + 1;
        }
        if (checkLeftTile < 0) checkLeftTile = 0;
        if (checkRightTile >= mapWidthTiles) checkRightTile = mapWidthTiles - 1;

        for (int tx = checkLeftTile; tx <= checkRightTile && !landed; tx++) {
            for (int ty = (int)(hitboxBottomY / tileSize); ty <= nextFeetTileY; ty++) {
                if (scene.IsSolid(tx, ty)) {
                    int tileOffsetY = scene.GetVisualOffsetY(tx, ty);
                    float platformTop = ty * tileSize + tileOffsetY;

                    if (nextFeetY >= platformTop && hitboxBottomY <= platformTop + 5) {
                        groundY = platformTop - (currentOffsetY + currentHeight) * scale;
                        landed = true;
                        break;
                    }
                }
            }
        }

        //移动平台碰撞检测
         // 检查是否站在移动平台上
        if (!landed && scene.HasMovingPlatforms()) {
            float movingPlatY;
            Rectangle feetHitbox = {
                position.x,
                nextY + (currentOffsetY + currentHeight) * scale - 4,
                currentTexture.width * scale,
                4
            };

            // 传递4个参数 
            bool hasButton = false;
            bool buttonCollected = false;
            int platformIndex = scene.CheckMovingPlatformCollision(feetHitbox, movingPlatY, hasButton, buttonCollected);

            if (platformIndex >= 0) {
                if (nextFeetY >= movingPlatY && hitboxBottomY <= movingPlatY + 5) {
                    groundY = movingPlatY - (currentOffsetY + currentHeight) * scale;
                    landed = true;
                    isOnMovingPlatform = true;
                    // 记录平台位置用于水平跟随
                    lastMovingPlatformPos = scene.GetNearestMovingPlatformPosition(GetHitbox());
                }
            }
        }

        //scene 2
        if (!landed) {
            float newPlatY;
            Rectangle feetHitbox = {
                position.x,
                nextY + (currentOffsetY + currentHeight) * scale - 4,
                currentTexture.width * scale,
                4
            };
            if (scene.CheckNewPlatformCollision(feetHitbox, newPlatY)) {
                if (nextFeetY >= newPlatY && hitboxBottomY <= newPlatY + 5) {
                    groundY = newPlatY - (currentOffsetY + currentHeight) * scale;
                    landed = true;
                }
            }
        }

        if (landed) {
            position.y = groundY;
            velocityY = 0;

            if (isJumping && wasInAir) {
                justLandedTimer = 0.12f;   // ventana de 120ms
                wasInAir = false;
            }

            isJumping = false;

            if (moveX == 0) ChangeState(PlayerState::IDLE);
            else ChangeState(PlayerState::WALKING);
        }
        else {
            if (isJumping) wasInAir = true;
            position.y = nextY;
        }
    }

    // Colisión al SALTAR (techo)
    if (velocityY < 0) {
        bool hitCeiling = false;
        float ceilingY = nextY;

        int checkLeftTile = leftTile;
        int checkRightTile = rightTile;

        if (scene.CheckAdjacentTiles()) {
            checkLeftTile = leftTile - 1;
            checkRightTile = rightTile + 1;
        }
        if (checkLeftTile < 0) checkLeftTile = 0;
        int mapWidthTiles = GameScene::GetScreenWidth() / tileSize;
        if (checkRightTile >= mapWidthTiles) checkRightTile = mapWidthTiles - 1;

        for (int tx = checkLeftTile; tx <= checkRightTile && !hitCeiling; tx++) {
            for (int ty = nextHeadTileY - 1; ty <= (int)(hitboxTopY / tileSize) + 1; ty++) {
                if (ty < 0) continue;

                if (scene.IsSolid(tx, ty)) {
                    int tileOffsetY = scene.GetVisualOffsetY(tx, ty);
                    float platformBottom = ty * tileSize + tileOffsetY + platformHitboxHeight;

                    if (nextHeadY <= platformBottom && hitboxTopY >= platformBottom - 5) {
                        ceilingY = platformBottom - currentOffsetY * scale;
                        hitCeiling = true;
                        break;
                    }
                }
            }
        }

        if (hitCeiling) {
            position.y = ceilingY;
            velocityY = 0;
        }
        else {
            position.y = nextY;
        }
    }
}

void Player::ActivateStarMode() {
    starMode = true;
    starModeTimer = starModeDuration;
    starCount = 0;
    currentTint = GOLD;
}
void Player::StartDeath() {
    if (isDying) return;
    isDying = true;
    deathTimer = 0.0f;
    deathFrame = 0;
    velocityY = 0;
    velocityX = 0;
    moveX = 0;
    moveY = 0;
    isJumping = false;
    onLadder = false;
    isClimbing = false;
    exitingLadder = false;
    // 播放死亡音效
    PlayDeathSound();
}

void Player::UpdateDeath(float deltaTime) {          
    if (!isDying) return;

    deathTimer += deltaTime;

    if (deathTimer < 0.25f) {
        deathFrame = 0;  // Death1
    }
    else if (deathTimer < 0.5f) {
        deathFrame = 1;  // Death2
    }
    else if (deathTimer < 0.75f) {
        deathFrame = 2;  // Death3
    }
    else if (deathTimer < 1.0f) {
        deathFrame = 3;  // Death4
    }
    else if (deathTimer < 2.25f) {
        deathFrame = 4;  // DeathEnd
    }
    else {
        isDying = false;
        deathTimer = 0.0f;
        deathFrame = 0;
    }
}

void Player::Respawn(int tileX, int tileY) {
    int tileSize = 32;
    int platformOffsetY = 8;

    position.x = (float)(tileX * tileSize);
    position.y = (float)(tileY * tileSize) + platformOffsetY
        - (baseHitboxOffsetY + baseHitboxHeight) * scale;

    velocityY = 0;
    velocityX = 0;
    isJumping = false;
    onLadder = false;
    isClimbing = false;
    exitingLadder = false;
    moveX = 0;
    moveY = 0;

    ChangeState(PlayerState::IDLE);
}

void Player::UpdateStarMode() {
    if (starMode) {
        starModeTimer -= GetFrameTime();

        if (starModeTimer <= 3.0f) {
            float blink = sin(starModeTimer * 20.0f);
            if (blink > 0) {
                currentTint = GOLD;
            }
            else {
                currentTint = WHITE;
            }
        }

        if (starModeTimer <= 0.0f) {
            starMode = false;
            starModeTimer = 0.0f;
            currentTint = WHITE;
        }
    }
}
    //得分系统 
void Player::AddScore(int points)
{
    score += points;

    if (bonusSoundLoaded) {
        PlaySound(bonusSound);
    }

    // 添加浮动文字 
    Vector2 textPos = GetFeetWorldPos();
    textPos.y -= 30.0f;  // 显示在玩家上方
    AddFloatingText(textPos, "+", points);
}

void Player::ResetScore()
{
    score = 0;
}

void Player::AddFloatingText(Vector2 worldPos, const std::string& text, int points)
{
    std::string displayText = text + std::to_string(points);
    floatingTexts.emplace_back(worldPos, displayText, 0.8f);
}

void Player::UpdateFloatingTexts()
{
    float dt = GetFrameTime();
    for (auto it = floatingTexts.begin(); it != floatingTexts.end(); )
    {
        it->lifetime -= dt;
        it->alpha = it->lifetime / 0.8f;
        if (it->alpha < 0) it->alpha = 0;
        it->floatOffset -= 40.0f * dt;
        if (it->lifetime <= 0)
            it = floatingTexts.erase(it);
        else
            ++it;
    }
}

void Player::SetHasHammer(bool has) {
    hasHammer = has;
    if (has) {
        isHammerOnCooldown = false;
        hammerCooldownTimer = 0.0f;
    }
}

void Player::DrawFloatingTexts()
{
    for (const auto& ft : floatingTexts)
    {
        Color textColor = { 255, 255, 255, (unsigned char)(ft.alpha * 255) };  // ← 改成白色
        Vector2 drawPos = { ft.position.x, ft.position.y + ft.floatOffset };
        int fontSize = 24;
        int textWidth = MeasureText(ft.text.c_str(), fontSize);
     
        // 白色主文字
        DrawText(ft.text.c_str(), (int)drawPos.x - textWidth / 2, (int)drawPos.y - 1, fontSize, textColor);
    }       
}

//锤子系统方法实现 
void Player::StartSwingHammer() {
    if (!hasHammer) return;  // 没有锤子就不能挥
    if (isHammerOnCooldown) return;   // 冷却中不能挥锤
    isSwingingHammer = true;
    swingHammerTimer = swingHammerDuration;  // 设置挥锤时间为0.3秒
    swingFrame = 0;
    ChangeState(PlayerState::HAMMER_SWING);  // 切换到挥锤状态
}

void Player::UpdateHammerSwing(float deltaTime) {
    if (!isSwingingHammer) return;

    swingHammerTimer -= deltaTime;

    // 计算动画帧
    float frameTime = swingHammerDuration / hammerSwingTextures.size();
    int newFrame = (int)((swingHammerDuration - swingHammerTimer) / frameTime);
    if (newFrame >= (int)hammerSwingTextures.size()) {
        newFrame = hammerSwingTextures.size() - 1;
    }
    swingFrame = newFrame;

    if (swingHammerTimer <= 0) {
        isSwingingHammer = false;
        swingFrame = 0;
        // ========== 挥锤结束，进入冷却 ==========
        isHammerOnCooldown = true;
        hammerCooldownTimer = hammerCooldownDuration;

        if (currentState == PlayerState::HAMMER_SWING) {
            ChangeState(PlayerState::IDLE);
        }
    }
}

Rectangle Player::GetAttackHitbox() const {
    // 攻击范围：玩家前方40像素，高度和玩家一样
    float attackRange = 40.0f;
    float attackHeight = currentTexture.height * scale;

    if (facingRight) {
        // 面向右时，攻击范围在玩家右边
        return {
            position.x + currentTexture.width * scale,  // 从玩家右侧开始
            position.y,                                   // 和玩家同高
            attackRange,                                  // 宽度40
            attackHeight                                  // 高度和玩家一样
        };
    }
    else {
        // 面向左时，攻击范围在玩家左边
        return {
            position.x - attackRange,  // 从玩家左侧开始
            position.y,
            attackRange,
            attackHeight
        };
    }
}
void Player::SetScore(int score) {
    this->score = score;  // 假设你的分数变量名是 score

}

void Player::Draw() {

    if (isDying) {
        Texture2D tex;
        if (deathFrame < 4) {
            tex = deathTextures[deathFrame];
        }
        else {
            tex = deathEndTexture;
        }
        DrawTextureEx(tex, position, 0.0f, scale, WHITE);
        return;  // No dibujar nada más durante la muerte
    }

    Rectangle source = { 0, 0, (float)currentTexture.width, (float)currentTexture.height };
    Rectangle dest = {
        position.x,
        position.y,
        currentTexture.width * scale,
        currentTexture.height * scale
    };
    Vector2 origin = { 0, 0 };

    if (!facingRight) {
        source.width = -source.width;
    }

    DrawTexturePro(currentTexture, source, dest, origin, 0.0f, currentTint);

    // ========== 绘制锤子 ==========
    if (isSwingingHammer && !hammerSwingTextures.empty()) {
        Vector2 hammerPos;
        int texIndex = swingFrame;
        if (texIndex >= (int)hammerSwingTextures.size()) {
            texIndex = hammerSwingTextures.size() - 1;
        }

        if (facingRight) {
            hammerPos = { position.x + 30, position.y };
            DrawTextureEx(hammerSwingTextures[texIndex], hammerPos, 0.0f, scale, WHITE);
        }
        else {
            hammerPos = { position.x - 30, position.y };
            // 正确翻转图片
            Rectangle src = { 0, 0, (float)hammerSwingTextures[texIndex].width, (float)hammerSwingTextures[texIndex].height };
            Rectangle dst = { hammerPos.x, hammerPos.y, hammerSwingTextures[texIndex].width * scale, hammerSwingTextures[texIndex].height * scale };
            src.width = -src.width;  // 水平翻转
            DrawTexturePro(hammerSwingTextures[texIndex], src, dst, { 0, 0 }, 0.0f, WHITE);
        }

    }


   // ========== INDICADORES (estrellas + martillo) ==========
    float iconScale = 1.9f;
    float rightX = GameScene::GetScreenWidth() - 120;
    float iconY = 100;

    // Estrellas
    Rectangle starDst = { rightX, iconY, starIconTexture.width * iconScale, starIconTexture.height * iconScale };
    DrawTexturePro(starIconTexture, { 0, 0, (float)starIconTexture.width, (float)starIconTexture.height }, starDst, { 0, 0 }, 0.0f, WHITE);
    DrawText(TextFormat("%d/%d", starCount, maxStars), starDst.x + starDst.width + 8, starDst.y + 5, 20, YELLOW);

    iconY += starDst.height + 3;

    // Martillo
    Rectangle hammerDst = { rightX, iconY, hammerIconTexture.width * iconScale, hammerIconTexture.height * iconScale };
    DrawTexturePro(hammerIconTexture, { 0, 0, (float)hammerIconTexture.width, (float)hammerIconTexture.height }, hammerDst, { 0, 0 }, 0.0f, WHITE);

    float textY = hammerDst.y + hammerDst.height / 2 - 4.5;  // Centrado verticalmente

    if (hasHammer) {
        if (isHammerOnCooldown) {
            DrawText(TextFormat("%.1fs", hammerCooldownTimer), hammerDst.x + hammerDst.width + 8, textY, 20, GRAY);
        }
        else {
            DrawText("READY", hammerDst.x + hammerDst.width + 8, textY, 20, ORANGE);
        }
    }
    else {
        DrawText("NO", hammerDst.x + hammerDst.width + 8, textY, 20, GRAY);
    }

    DrawFloatingTexts();   // 绘制浮动文字 
}

void Player::PlayDeathSound() {
    if (deathSoundLoaded) {
        StopSound(deathSound);  // 停止之前的播放（如果有）
        PlaySound(deathSound);
        TraceLog(LOG_INFO, "Playing player death sound");
    }
}


// 新增：移动平台相关方法 
void Player::SetOnMovingPlatform(bool on, const Vector2& platformPos)
{
    isOnMovingPlatform = on;
    if (on) {
        lastMovingPlatformPos = platformPos;
    }
}