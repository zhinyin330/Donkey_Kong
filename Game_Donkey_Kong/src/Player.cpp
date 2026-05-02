#include "Player.h"
#include "GameScene.h"  
#include "resource_dir.h"

//Para usarlo correctamente; Duracion del modo immune
const float Player::starModeDuration = 10.0f;

Player::Player() {
    // Cargar texturas
    idleTexture = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png");
    jumpTexture = LoadTexture("Characters/Mario/Dk_Mario_Jump.png");
    jumpSound = LoadSound("audio/SFXjump.mp3");
    SetSoundVolume(jumpSound, 5.0f);

    // Pasos
    walkSound = LoadSound("audio/Walking.mp3");
    SetSoundVolume(walkSound, 5.0f);

    stepTimer = 0.0f;
    stepInterval = 0.5f;

    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk1.png"));
    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk2.png"));
    walkEndTexture = LoadTexture("Characters/Mario/Dk_Mario_WalkEnd.png");

    climbTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Ladder1.png"));
    climbTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Ladder2.png"));

    climbEndTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_LadderEnd1.png"));
    climbEndTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_LadderEnd2.png"));

    // Variables
    currentTexture = idleTexture;
    speed = 5.0f;
    velocityY = 0.0f;
    gravity = 0.3f;
    isJumping = false;
    scale = 2.3f;
    moveX = 0.0f;
    velocityX = 0.0f;
    onLadder = false;
    isClimbing = false;
    climbSpeed = 2.5f;
    moveY = 0.0f;
    exitingLadder = false;

    // Configuración de hitbox
    baseHitboxOffsetY = 2;
    baseHitboxHeight = 14;

    int feetOffset = baseHitboxOffsetY + baseHitboxHeight;
    jumpHitboxOffsetY = 0;
    jumpHitboxHeight = feetOffset - jumpHitboxOffsetY;

    int tileSize = 32;
    int startTileX = 2;
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

    // Star
    starCount = 0;

    // Modo estrella
    starMode = false;
    starModeTimer = 0.0f;
    currentTint = WHITE;
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
}

void Player::HandleInput(GameScene& scene) {
    if (exitingLadder) {
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
            velocityY = -7.5f;
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
        if (IsKeyPressed(KEY_Z) && !starMode && starCount >= maxStars) {
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
        }

        if (!isJumping || currentState == PlayerState::JUMPING) {
            SetFeetPosition(feetY);
        }
    }
}

void Player::UpdateAnimation() {
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
    UpdateAnimation();
    UpdateStarMode();

    // ================= SISTEMA DE PASOS =================
    bool isMovingOnGround =
        (currentState == PlayerState::WALKING) &&
        !isJumping &&
        !onLadder &&
        (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) ||
            IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT));

    if (wasMoving && !isMovingOnGround) {
        stepTimer = 0.0f;
        isStepPlaying = false;
    }

    if (isMovingOnGround) {
        stepTimer += GetFrameTime();

        if (stepTimer >= stepInterval && !isStepPlaying) {
            PlaySound(walkSound);
            stepTimer = 0.0f;
            isStepPlaying = true;
        }
        else if (stepTimer < stepInterval) {
            isStepPlaying = false;
        }
    }
    else {
        stepTimer = 0.0f;
        isStepPlaying = false;
    }

    wasMoving = isMovingOnGround;

    // Si está en animación de salida, solo animación
    if (currentState == PlayerState::CLIMBING_END) {
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

        int checkLeftTile = leftTile - 1;
        int checkRightTile = rightTile + 1;
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

        if (landed) {
            position.y = groundY;
            velocityY = 0;
            isJumping = false;

            if (moveX == 0) ChangeState(PlayerState::IDLE);
            else ChangeState(PlayerState::WALKING);
        }
        else {
            position.y = nextY;
        }
    }

    // Colisión al SALTAR (techo)
    if (velocityY < 0) {
        bool hitCeiling = false;
        float ceilingY = nextY;

        int checkLeftTile = leftTile - 1;
        int checkRightTile = rightTile + 1;
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

void Player::Draw() {
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

    // Debug: Hitbox
    int currentOffsetY = GetCurrentHitboxOffsetY();
    int currentHeight = GetCurrentHitboxHeight();

    DrawRectangleLines(
        position.x,
        position.y + currentOffsetY * scale,
        currentTexture.width * scale,
        currentHeight * scale,
        RED
    );

    DrawText(TextFormat("Estrellas: %d/%d", starCount, maxStars),
        GameScene::GetScreenWidth() - 200, 680, 20, YELLOW);
}