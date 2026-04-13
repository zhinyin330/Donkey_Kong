#include "Player.h"
#include "Scene.h"
#include "resource_dir.h" 

Player::Player() {
    // Cargar texturas
    idleTexture = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png");
    jumpTexture = LoadTexture("Characters/Mario/Dk_Mario_Jump.png");
    jumpSound = LoadSound("audio/SFXjump.mp3");
    SetSoundVolume(jumpSound, 5.0f);

    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk1.png"));
    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk2.png"));
    walkEndTexture = LoadTexture("Characters/Mario/Dk_Mario_WalkEnd.png");

    currentTexture = idleTexture;
    speed = 5.0f;
    velocityY = 0.0f;
    gravity = 0.3f;
    isJumping = false;
    scale = 2.1f;
    moveX = 0.0f;

    // Configuración CRÍTICA de hitbox
    baseHitboxOffsetY = 4;
    baseHitboxHeight = 12;

    // Asegurar que los pies estén en la misma posición relativa
    // offsetY + height debe ser IGUAL para todos los estados
    int feetOffset = baseHitboxOffsetY + baseHitboxHeight;  // = 16
    jumpHitboxOffsetY = 0;  // Ajusta según tu sprite de salto
    jumpHitboxHeight = feetOffset - jumpHitboxOffsetY;  // = 13

    int tileSize = 32;  // tileSize * tileScale
    int startTileX = 2; // Columna donde aparece Mario
    int startTileY = 21; // Suelo principal

    // FÓRMULA para la posición Y con hitbox rectangular
    int platformOffsetY = 16;  // baseOffset = 4 * tileScale = 8
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
}

void Player::HandleInput() {
    moveX = 0;

    if (IsKeyDown(KEY_A)) {
        moveX = -1;
        facingRight = false;
    }
    if (IsKeyDown(KEY_D)) {
        moveX = 1;
        facingRight = true;
    }

    if (IsKeyPressed(KEY_SPACE) && !isJumping) {
        velocityY = -8.0f;
        isJumping = true;
        PlaySound(jumpSound);
    }

    // DEBUG
    static int lastMoveX = 0;
    if (moveX != lastMoveX) {
        lastMoveX = moveX;
    }

    if (isJumping) {
        ChangeState(PlayerState::JUMPING);
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

Player::~Player() {
    // Liberar todas las texturas
    UnloadTexture(idleTexture);
    UnloadTexture(jumpTexture);
    UnloadTexture(walkEndTexture);
    //audio
    UnloadSound(jumpSound);
    for (Texture2D& tex : walkTextures) {
        UnloadTexture(tex);
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
        // DEBUG
        const char* stateNames[] = { "IDLE", "WALKING", "WALK_END", "JUMPING" };

        float feetY = GetFeetPosition();
        currentState = newState;
        currentFrame = 0;
        frameCounter = 0;
        walkEndCounter = 0;

        switch (currentState) {
        case PlayerState::IDLE:
            currentTexture = idleTexture;
            break;
        case PlayerState::WALKING:
            if (!walkTextures.empty()) {
                currentTexture = walkTextures[0];
            }
            break;
        case PlayerState::WALK_END:
            currentTexture = walkEndTexture;
            break;
        case PlayerState::JUMPING:
            currentTexture = jumpTexture;
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
            if (currentFrame >= walkTextures.size()) {
                currentFrame = 0;
            }
            currentTexture = walkTextures[currentFrame];
            break;

        case PlayerState::WALK_END:
            walkEndCounter++;
            if (walkEndCounter >= 60) {
                ChangeState(PlayerState::IDLE);
            }
   
            break;

        case PlayerState::JUMPING:
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

    // Verificar también el tile actual (no solo el siguiente)
    int currentLeftTile = (int)(position.x / tileSize);
    int currentRightTile = (int)((position.x + currentTexture.width * scale - 1) / tileSize);

    if (moveX < 0) {
        for (int ty = topTile; ty <= bottomTile + 1 && !horizontalCollision; ty++) {  // ← +1 para esquinas
            if (scene.IsSolid(leftTile, ty)) {
                int tileOffsetY = scene.GetVisualOffsetY(leftTile, ty);
                float platformTop = ty * tileSize + tileOffsetY;
                float platformBottom = platformTop + platformHitboxHeight;

                float feetLevel = hitboxBottomY;
                float headLevel = hitboxTopY;

                // Verificar colisión con esquina inferior
                if (platformTop < headLevel + 8 && platformBottom > feetLevel - 8) {
                    // Si la plataforma está más baja que sus pies, permitir pasar (rampa)
                    if (platformTop <= feetLevel + 4 && moveX < 0) {
                        // Verificar si hay plataforma SÓLIDA debajo
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

                float feetLevel = hitboxBottomY;
                float headLevel = hitboxTopY;

                // Verificar si estamos JUSTO en el borde
                if (platformTop < headLevel + 8 && platformBottom > feetLevel - 8) {
                    // Si la plataforma está a la altura del cuerpo, bloquear
                    if (platformTop > feetLevel - 4) {
                        nextX = (float)(currentLeftTile + 1) * tileSize;
                        horizontalCollision = true;
                        break;
                    }
                }
            }
        }
    }
    else if (moveX > 0) {
        for (int ty = topTile; ty <= bottomTile + 1 && !horizontalCollision; ty++) {  // ← +1 para esquinas
            if (scene.IsSolid(rightTile, ty)) {
                int tileOffsetY = scene.GetVisualOffsetY(rightTile, ty);
                float platformTop = ty * tileSize + tileOffsetY;
                float platformBottom = platformTop + platformHitboxHeight;

                float feetLevel = hitboxBottomY;
                float headLevel = hitboxTopY;

                if (platformTop < headLevel + 8 && platformBottom > feetLevel - 8) {
                    if (platformTop <= feetLevel + 4 && moveX > 0) {
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

                float feetLevel = hitboxBottomY;
                float headLevel = hitboxTopY;

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

    // Colisión al SALTAR (golpear techo)
    if (velocityY < 0) {
        bool hitCeiling = false;
        float ceilingY = nextY;

        for (int tx = leftTile; tx <= rightTile && !hitCeiling; tx++) {
            for (int ty = nextHeadTileY; ty <= (int)(hitboxTopY / tileSize); ty++) {
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

    // NUEVO: Dibujar línea de pies (debe tocar exactamente el suelo)
    float feetY = position.y + (currentOffsetY + currentHeight) * scale;
    DrawLine(
        position.x,
        feetY,
        position.x + currentTexture.width * scale,
        feetY,
        GREEN
    );

    // NUEVO: Dibujar línea superior de hitbox
    float hitboxTopY = position.y + currentOffsetY * scale;
    DrawLine(
        position.x,
        hitboxTopY,
        position.x + currentTexture.width * scale,
        hitboxTopY,
        BLUE
    );



}