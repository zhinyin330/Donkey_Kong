#include "Player.h"
#include "Player.h"
#include "resource_dir.h" 

Player::Player() {
    texture = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png"); // tu sprite en reposo

    position = { 100.0f, 300.0f };
    speed = 4.0f;
}

void Player::HandleInput() {
    // Movimiento
    if (IsKeyDown(KEY_A)) {
        position.x -= speed;
    }
    if (IsKeyDown(KEY_D)) {
        position.x += speed;
    }

    // Saltar
    if (IsKeyPressed(KEY_SPACE) && !isJumping) {
        velocityY = -10.0f; // fuerza del salto
        isJumping = true;
    }
}

void Player::Update(Scene& scene) {

    int tileSize = scene.GetTileSize();

    // gravedad
    velocityY += gravity;

    // siguiente posición en Y
    float nextY = position.y + velocityY;

    // calcular tiles debajo del jugador
    int leftTile = (int)(position.x / tileSize);
    int rightTile = (int)((position.x + texture.width * scale) / tileSize);
    int bottomTile = (int)((nextY + texture.height * scale) / tileSize);

    // comprobar colisión suelo
    if (scene.IsSolid(leftTile, bottomTile) ||
        scene.IsSolid(rightTile, bottomTile)) {

        // ajustar al borde del bloque
        position.y = bottomTile * tileSize - (texture.height * scale);

        velocityY = 0;
        isJumping = false;
    }
    else {
        position.y = nextY;
        isJumping = true;
    }
}

void Player::Draw() {
    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };

    float scale = 2.5f; // AJUSTA ESTO

    Rectangle dest = {
        position.x,
        position.y,
        texture.width * scale,
        texture.height * scale
    };

    DrawTexturePro(texture, source, dest, { 0,0 }, 0.0f, WHITE);
}