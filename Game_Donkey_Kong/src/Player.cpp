#include "Player.h"
#include "Scene.h"
#include "resource_dir.h" 

Player::Player() {
    // Cargar texturas
    idleTexture = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png");
    jumpTexture = LoadTexture("Characters/Mario/Dk_Mario_Jump.png");

    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk1.png"));
    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_Walk2.png"));
    walkTextures.push_back(LoadTexture("Characters/Mario/Dk_Mario_WalkEnd.png"));

    currentTexture = idleTexture;
    speed = 7.0f;
    velocityY = 0.0f;
    gravity = 0.2f;
    isJumping = false;
    scale = 2.0f;
    moveX = 0.0f;

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
    int platformSolidOffsetY = 16; // platformHitboxOffsetY * tileScale (8 * 2)
    position.x = (float)(startTileX * tileSize);
    position.y = (float)(startTileY * tileSize) + platformSolidOffsetY
        - (baseHitboxOffsetY + baseHitboxHeight) * scale;

    // Animación
    currentState = PlayerState::IDLE;
    currentFrame = 0;
    frameCounter = 0;
    frameSpeed = 0.15f;
    facingRight = true;
}

Player::~Player() {
    // Liberar todas las texturas
    UnloadTexture(idleTexture);
    UnloadTexture(jumpTexture);
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
        // GUARDAR la posición de los pies antes de cambiar
        float feetY = GetFeetPosition();

        currentState = newState;
        currentFrame = 0;
        frameCounter = 0;

        // Actualizar textura
        switch (currentState) {
        case PlayerState::IDLE:
            currentTexture = idleTexture;
            break;
        case PlayerState::WALKING:
            if (!walkTextures.empty()) {
                currentTexture = walkTextures[0];
            }
            break;
        case PlayerState::JUMPING:
            currentTexture = jumpTexture;
            break;
        }

        // RESTAURAR la posición de los pies (solo si está en el suelo)
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
            // No hay animación en idle (solo un frame)
            break;

        case PlayerState::WALKING:
            // Ciclar entre los frames de caminar
            currentFrame++;
            if (currentFrame >= walkTextures.size()) {
                currentFrame = 0;
            }
            currentTexture = walkTextures[currentFrame];
            break;

        case PlayerState::JUMPING:
            // Solo un frame para saltar
            break;
        }
    }
}

void Player::HandleInput() {
    // Movimiento horizontal
    moveX = 0;

    if (IsKeyDown(KEY_A)) {
        moveX = -1;
        facingRight = false;
    }
    if (IsKeyDown(KEY_D)) {
        moveX = 1;
        facingRight = true;
    }

    // Saltar
    if (IsKeyPressed(KEY_SPACE) && !isJumping) {
        velocityY = -8.5f;
        isJumping = true;
    }

    // Determinar estado actual para la animación
    if (isJumping) {
        ChangeState(PlayerState::JUMPING);
    }
    else if (moveX != 0) {
        ChangeState(PlayerState::WALKING);
    }
    else {
        ChangeState(PlayerState::IDLE);
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

    int tileSize = scene.GetTileSize();  // 32
    int currentOffsetY = GetCurrentHitboxOffsetY();
    int currentHeight = GetCurrentHitboxHeight();

    // Dimensiones de la hitbox de plataforma
    int platformHitboxHeight = scene.GetPlatformHitboxHeight();    // 8px * escala 2 = 16px
    int platformHitboxOffsetY = scene.GetPlatformHitboxOffsetY();  // 4px * escala 2 = 8px

    // HORIZONTAL 
    float nextX = position.x + moveX * speed;

    int leftTile = (int)(nextX / tileSize);
    int rightTile = (int)((nextX + currentTexture.width * scale - 1) / tileSize);

    float hitboxTopY = position.y + currentOffsetY * scale;
    float hitboxBottomY = position.y + currentOffsetY * scale + currentHeight * scale;

    int topTile = (int)(hitboxTopY / tileSize);
    int bottomTile = (int)((hitboxBottomY - 1) / tileSize);

    // LÍMITES DEL MAPA (no salir por los bordes)
    int mapWidthPixels = Scene::GetScreenWidth();
    if (nextX < 0) {
        nextX = 0;
    }
    if (nextX + currentTexture.width * scale > mapWidthPixels) {
        nextX = mapWidthPixels - currentTexture.width * scale;
    }

    // Colisión horizontal (usando la hitbox rectangular de la plataforma)
    if (moveX < 0) {
        for (int ty = topTile; ty <= bottomTile; ty++) {
            if (scene.IsSolid(leftTile, ty)) {
                // Verificar si la hitbox del jugador intersecta con la parte sólida de la plataforma
                int tileOffsetY = scene.GetVisualOffsetY(leftTile, ty);
                float platformTop = ty * tileSize + tileOffsetY;
                float platformBottom = platformTop + platformHitboxHeight;

                if (hitboxBottomY > platformTop && hitboxTopY < platformBottom) {
                    nextX = (float)(leftTile + 1) * tileSize;
                    break;
                }
            }
        }
    }
    else if (moveX > 0) {
        for (int ty = topTile; ty <= bottomTile; ty++) {
            if (scene.IsSolid(rightTile, ty)) {
                int tileOffsetY = scene.GetVisualOffsetY(rightTile, ty);
                float platformTop = ty * tileSize + tileOffsetY;
                float platformBottom = platformTop + platformHitboxHeight;

                if (hitboxBottomY > platformTop && hitboxTopY < platformBottom) {
                    nextX = (float)(rightTile * tileSize) - currentTexture.width * scale;
                    break;
                }
            }
        }
    }

    position.x = nextX;

    // VERTICAL - Colisión con hitbox rectangular
    velocityY += gravity;
    float nextY = position.y + velocityY;

    float nextHitboxTopY = nextY + currentOffsetY * scale;
    float nextHitboxBottomY = nextY + currentOffsetY * scale + currentHeight * scale;

    int nextTopTile = (int)(nextHitboxTopY / tileSize);
    int nextBottomTile = (int)(nextHitboxBottomY / tileSize);

    leftTile = (int)(position.x / tileSize);
    rightTile = (int)((position.x + currentTexture.width * scale) / tileSize);

    // LÍMITE INFERIOR
    int mapHeightPixels = Scene::GetScreenHeight();
    if (nextHitboxBottomY > mapHeightPixels) {
        position.x = 2 * tileSize;
        position.y = 21 * tileSize + 16 - (baseHitboxOffsetY + baseHitboxHeight) * scale;
        velocityY = 0;
        isJumping = false;
        ChangeState(PlayerState::IDLE);
        return;
    }

    // LÍMITE SUPERIOR
    if (nextHitboxTopY < 0) {
        position.y = -currentOffsetY * scale;
        velocityY = 0;
        return;
    }

    // Caída (colisión con parte superior de plataforma)
    if (velocityY > 0) {
        for (int tx = leftTile; tx <= rightTile; tx++) {
            if (scene.IsSolid(tx, nextBottomTile)) {
                int tileOffsetY = scene.GetVisualOffsetY(tx, nextBottomTile);
                float platformTop = nextBottomTile * tileSize + tileOffsetY;

              
                if (nextHitboxBottomY >= platformTop && hitboxBottomY <= platformTop) {
                    // Mario está cayendo y va a colisionar con la plataforma
                    position.y = platformTop - currentOffsetY * scale - currentHeight * scale;
                    velocityY = 0;
                    isJumping = false;

                    if (moveX == 0) ChangeState(PlayerState::IDLE);
                    else ChangeState(PlayerState::WALKING);
                    return;
                }
            }
        }
    }

    // Salto (colisión con parte inferior de plataforma)
    if (velocityY < 0) {
        for (int tx = leftTile; tx <= rightTile; tx++) {
            if (scene.IsSolid(tx, nextTopTile)) {
                int tileOffsetY = scene.GetVisualOffsetY(tx, nextTopTile);
                float platformBottom = nextTopTile * tileSize + tileOffsetY + platformHitboxHeight;

                if (nextHitboxTopY <= platformBottom && hitboxTopY > platformBottom) {
                    // Mario está subiendo y va a golpear el techo
                    position.y = platformBottom - currentOffsetY * scale;
                    velocityY = 0;
                    return;
                }
            }
        }
    }

    position.y = nextY;
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