#include "Player.h"
#include "Scene.h"
#include "resource_dir.h" 

Player::Player() {
    // Cargar texturas
    idleTexture = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png");
    jumpTexture = LoadTexture("Characters/Mario/Dk_Mario_Jump.png");
    jumpSound = LoadSound("audio/SFXjump.mp3");
    SetSoundVolume(jumpSound, 5.0f);

    // 脚步声加载（加在这里）
    walkSound = LoadSound("audio/Walking.mp3");
    SetSoundVolume(walkSound, 5.0f);

    //  脚步节奏初始化
    stepTimer = 0.0f;
    stepInterval = 0.5f; // 可以调：越小走路越快                              

    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk1.png"));
    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk2.png"));
    walkEndTexture = LoadTexture("Characters/Mario/Dk_Mario_WalkEnd.png");

    climbTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Ladder1.png"));
    climbTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Ladder2.png"));

    climbEndTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_LadderEnd1.png"));
    climbEndTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_LadderEnd2.png"));

    //variables
    currentTexture = idleTexture;
    speed = 5.0f;
    velocityY = 0.0f;
    gravity = 0.3f;
    isJumping = false;
    scale = 2.5f;
    moveX = 0.0f;
    velocityX = 0.0f;
    onLadder = false;
    isClimbing = false;
    climbSpeed = 2.5f;
    moveY = 0.0f;
    exitingLadder = false;

    // Configuración CRÍTICA de hitbox
    baseHitboxOffsetY = 2;
    baseHitboxHeight = 14;

    // Asegurar que los pies estén en la misma posición relativa
    // offsetY + height debe ser IGUAL para todos los estados
    int feetOffset = baseHitboxOffsetY + baseHitboxHeight;  // = 16
    jumpHitboxOffsetY = 0;  // Ajusta según tu sprite de salto
    jumpHitboxHeight = feetOffset - jumpHitboxOffsetY;  // = 13

    int tileSize = 32;  // tileSize * tileScale
    int startTileX = 2; // Columna donde aparece Mario
    int startTileY = 21; // Suelo principal

    // FÓRMULA para la posición Y con hitbox rectangular
    int platformOffsetY = 8;  // baseOffset = 4 * tileScale = 8
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
}

Player::~Player() {
    // Liberar todas las texturas
    UnloadTexture(idleTexture);
    UnloadTexture(jumpTexture);

    UnloadSound(walkSound);//audio walking
    UnloadTexture(walkEndTexture);
    for (Texture2D& tex : walkTextures) UnloadTexture(tex);
    for (Texture2D& tex : climbTextures) UnloadTexture(tex);
    for (Texture2D& tex : climbEndTextures) UnloadTexture(tex);

    //audio
    UnloadSound(jumpSound);
}

void Player::HandleInput(Scene& scene) {
    
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



    // SOLO entrar en modo escalera si presiona ↑/↓
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
            //  Si YA estaba en CLIMBING y suelta teclas, seguir en CLIMBING pero quieto
            isClimbing = false;  // No hay movimiento vertical
            moveY = 0;
            // Mantener CLIMBING (no cambiar a IDLE)
        }
        else {
            // Toca escalera pero NO presiona y NO estaba en CLIMBING → IDLE normal
            onLadder = false;  // Para que no afecte la física
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

    // Movimiento normal (si no está en CLIMBING)
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
    }

    // Determinar estado
    if (exitingLadder) {
        // Mantener CLIMBING_END
    }
    else if (isJumping) {
        ChangeState(PlayerState::JUMPING);
    }
    else if (currentState == PlayerState::CLIMBING) {
        // Mantener CLIMBING (ya se estableció antes)
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
                currentFrame = 0;      // Asegurar que empiece desde el primer frame
                frameCounter = 0;      // Resetear contador
                walkEndCounter = 0;    // Resetear contador
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
            // Ciclo de escalada
            if (isClimbing) {
                currentFrame++;
                if (currentFrame >= climbTextures.size()) currentFrame = 0;
                currentTexture = climbTextures[currentFrame];
            }
            break;

        case PlayerState::CLIMBING_END:
            // Secuencia de salida (una sola vez)
            currentFrame++;
            if (currentFrame >= climbEndTextures.size()) {
                // Terminó la animación de salida
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

void Player::Update(Scene& scene) {
    UpdateAnimation();

    // ================= 脚步声系统 =================
    bool isMovingOnGround =
        (currentState == PlayerState::WALKING) &&
        !isJumping &&
        !onLadder;
                                                                   
    (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) ||
        IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT));  // ← Verificador directo

    if (isMovingOnGround) {
        TraceLog(LOG_INFO, "Caminando - Timer: %.2f", stepTimer);
    }

    // Detectar cuando se ACABA de mover
    if (wasMoving && !isMovingOnGround) {
        stepTimer = 0.0f;
        isStepPlaying = false;
    }


    // 节奏脚步声（推荐）
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

    // ============================================

    // Si está en animación de salida, solo animación
    if (currentState == PlayerState::CLIMBING_END) {
        velocityY = 0;
        isJumping = false;
        return;
    }

    // Si está escalando activamente
    if (currentState == PlayerState::CLIMBING && !exitingLadder) {
        // Movimiento vertical
        if (isClimbing) {
            position.y += moveY * climbSpeed;
        }
        position.x += moveX * speed;

        // Límites
        if (position.x < 0) position.x = 0;
        if (position.x + currentTexture.width * scale > Scene::GetScreenWidth()) {
            position.x = Scene::GetScreenWidth() - currentTexture.width * scale;
        }
        if (position.y < 0) position.y = 0;
        if (position.y + currentTexture.height * scale > Scene::GetScreenHeight()) {
            position.y = Scene::GetScreenHeight() - currentTexture.height * scale;
        }

        velocityY = 0;
        isJumping = false;

        int centerX = (int)(position.x + currentTexture.width * scale / 2);
        int tileX = centerX / 32;
        int feetY = (int)(position.y + (baseHitboxOffsetY + baseHitboxHeight) * scale);
        int tileY = feetY / 32;

        // SOLO verificar hitbox de escalera
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

    // Límites del mapa
    int mapWidthPixels = Scene::GetScreenWidth();
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

    // Determinar dirección del movimiento horizontal
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

    // Verificar colisión lateral incluso SIN movimiento horizontal
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

    // ========== PARED LATERAL AGRESIVA ==========
    if (!horizontalCollision) {
        int mapWidthTiles = Scene::GetScreenWidth() / tileSize;
        int mapHeightTiles = Scene::GetScreenHeight() / tileSize;

        // Recorrer desde la cabeza hasta los pies
        for (int ty = topTile; ty <= bottomTile; ty++) {
            if (ty < 0 || ty >= mapHeightTiles) continue;

            // ===== BORDE IZQUIERDO (plataforma a la izquierda de Mario) =====
            int leftCheckX = currentLeftTile - 1;
            if (leftCheckX >= 0) {
                // Verificar si hay una plataforma SÓLIDA a la izquierda
                if (scene.IsSolid(leftCheckX, ty)) {
                    int tileOffsetY = scene.GetVisualOffsetY(leftCheckX, ty);
                    float platformTop = ty * tileSize + tileOffsetY;
                    float platformBottom = platformTop + platformHitboxHeight;

                    // Si la plataforma está a la altura del cuerpo de Mario
                    if (hitboxBottomY > platformTop + 2 && hitboxTopY < platformBottom - 2) {
                        float tileRight = (leftCheckX + 1) * tileSize;

                        // VERIFICACIÓN MÁS ESTRICTA: Si Mario está A LA IZQUIERDA de la plataforma
                        // pero su hitbox se solapa, empujarlo FUERTE
                        if (position.x < tileRight && position.x + currentTexture.width * scale > leftCheckX * tileSize) {
                            // Empujar a Mario completamente fuera
                            nextX = tileRight;
                            horizontalCollision = true;
                            break;
                        }
                    }
                }
            }

            // ===== BORDE DERECHO (plataforma a la derecha de Mario) =====
            int rightCheckX = currentRightTile + 1;
            if (!horizontalCollision && rightCheckX < mapWidthTiles) {
                if (scene.IsSolid(rightCheckX, ty)) {
                    int tileOffsetY = scene.GetVisualOffsetY(rightCheckX, ty);
                    float platformTop = ty * tileSize + tileOffsetY;
                    float platformBottom = platformTop + platformHitboxHeight;

                    if (hitboxBottomY > platformTop + 2 && hitboxTopY < platformBottom - 2) {
                        float tileLeft = rightCheckX * tileSize;
                        float marioRight = position.x + currentTexture.width * scale;

                        // VERIFICACIÓN MÁS ESTRICTA
                        if (marioRight > tileLeft && position.x < (rightCheckX + 1) * tileSize) {
                            // Empujar a Mario completamente fuera
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
    int mapHeightPixels = Scene::GetScreenHeight();
    if (nextFeetY > mapHeightPixels) {
        // Reiniciar al caer
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

        // Calcular el ancho del mapa en tiles
        int mapWidthTiles = Scene::GetScreenWidth() / tileSize;

        int checkLeftTile = leftTile - 1 ;
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

    // Colisión al SALTAR (golpear techo)
    if (velocityY < 0) {
        bool hitCeiling = false;
        float ceilingY = nextY;

        int checkLeftTile = leftTile - 1;
        int checkRightTile = rightTile + 1;
        if (checkLeftTile < 0) checkLeftTile = 0;
        int mapWidthTiles = Scene::GetScreenWidth() / tileSize;
        if (checkRightTile >= mapWidthTiles) checkRightTile = mapWidthTiles - 1;

        // USAR checkLeftTile y checkRightTile
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

    DrawTexturePro(currentTexture, source, dest, origin, 0.0f, WHITE);

    // Debug: Hitbox ajustada al estado actual
    int currentOffsetY = GetCurrentHitboxOffsetY();
    int currentHeight = GetCurrentHitboxHeight();

    // Debug: Hitbox
    DrawRectangleLines(
        position.x,
        position.y + currentOffsetY * scale,
        currentTexture.width * scale,
        currentHeight * scale,
        RED
    );

}