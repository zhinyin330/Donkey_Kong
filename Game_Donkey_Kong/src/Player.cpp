#include "Player.h"
#include "Scene.h"
#include "resource_dir.h" 

Player::Player() {
    texture = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png"); // tu sprite en reposo

    position = { 100.0f, 300.0f };
    speed = 4.0f;

    velocityY = 0.0f;
    gravity = 0.5f;
    isJumping = false;

    scale = 1.0f;
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

//collion
void Player::Update(Scene& scene) {

    int tileSize = scene.GetTileSize();

    // gravedad
    velocityY += gravity;

    // siguiente posición en Y
    float nextY = position.y + velocityY;

    //calcular tiles debajo del jugardor
    float playerBottom = nextY + texture.height * scale;

    int leftTile = (int)(position.x / tileSize);
    int rightTile = (int)((position.x + texture.width * scale) / tileSize);
    int bottomTile = (int)(playerBottom / tileSize);

    if (scene.IsSolid(leftTile, bottomTile) || scene.IsSolid(rightTile, bottomTile)) {

        // colocar justo encima del bloque
        position.y = bottomTile * tileSize - texture.height * scale;

        velocityY = 0;
        isJumping = false;
    }
    else {
        position.y = nextY;
    }
}

void Player::Draw() {
    DrawTextureEx(texture,position,0.0f,scale,WHITE);
}