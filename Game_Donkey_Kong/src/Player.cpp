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
    position = { 50.0f, 500.0f };
    speed = 4.0f;
    velocityY = 0.0f;
    gravity = 0.3f;
    isJumping = false;
    scale = 2.0f;
    moveX = 0.0f;

    // Configuración CRÍTICA de hitbox
    baseHitboxOffsetY = 5;
    baseHitboxHeight = 11;

    // Asegurar que los pies estén en la misma posición relativa
    // offsetY + height debe ser IGUAL para todos los estados
    int feetOffset = baseHitboxOffsetY + baseHitboxHeight;  // = 16

    jumpHitboxOffsetY = 3;  // Ajusta según tu sprite de salto
    jumpHitboxHeight = feetOffset - jumpHitboxOffsetY;  // = 13

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

    int tileSize = scene.GetTileSize();
    int currentOffsetY = GetCurrentHitboxOffsetY();
    int currentHeight = GetCurrentHitboxHeight();

    // HORIZONTAL
    float nextX = position.x + moveX * speed;

    // Calcular tiles basados en la HITBOX, no en la textura completa
    int leftTile = (int)(nextX / tileSize);
    int rightTile = (int)((nextX + currentTexture.width * scale - 1) / tileSize);

    // ¡CORRECCIÓN! Usar la posición Y de la hitbox, no de la textura
    float hitboxTopY = position.y + currentOffsetY * scale;
    float hitboxBottomY = position.y + currentOffsetY * scale + currentHeight * scale;

    int topTile = (int)(hitboxTopY / tileSize);
    int bottomTile = (int)((hitboxBottomY - 1) / tileSize);  // -1 para evitar overflow

    if (moveX < 0) {
        if (scene.IsSolid(leftTile, topTile) || scene.IsSolid(leftTile, bottomTile)) {
            nextX = (float)(leftTile + 1) * tileSize;
        }
    }
    else if (moveX > 0) {
        if (scene.IsSolid(rightTile, topTile) || scene.IsSolid(rightTile, bottomTile)) {
            nextX = (float)(rightTile * tileSize) - currentTexture.width * scale;
        }
    }

    position.x = nextX;

    // VERTICAL (esta parte está bien)
    velocityY += gravity;
    float nextY = position.y + velocityY;

    leftTile = (int)(position.x / tileSize);
    rightTile = (int)((position.x + currentTexture.width * scale) / tileSize);
    topTile = (int)((nextY + currentOffsetY * scale) / tileSize);
    bottomTile = (int)((nextY + currentOffsetY * scale + currentHeight * scale) / tileSize);

    if (velocityY > 0 &&
        (scene.IsSolid(leftTile, bottomTile) || scene.IsSolid(rightTile, bottomTile))) {

        position.y = (float)bottomTile * tileSize - currentOffsetY * scale - currentHeight * scale;
        velocityY = 0;
        isJumping = false;

        // Asegurar que el estado se actualice al aterrizar
        if (moveX == 0) {
            ChangeState(PlayerState::IDLE);
        }
        else {
            ChangeState(PlayerState::WALKING);
        }
    }
    else if (velocityY < 0 &&
        (scene.IsSolid(leftTile, topTile) || scene.IsSolid(rightTile, topTile))) {

        position.y = (float)(topTile + 1) * tileSize - currentOffsetY * scale;
        velocityY = 0;
    }
    else {
        position.y = nextY;
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