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

    scale = 1.5f;

    moveX = 0.0f;
}

void Player::HandleInput() {

    // Movimiento
    moveX = 0;
    if (IsKeyDown(KEY_A)) {
        moveX = -1;
    }
    if (IsKeyDown(KEY_D)) {
        moveX = +1;
    }

    // Saltar
    if (IsKeyPressed(KEY_SPACE) && !isJumping) {
        velocityY = -12.0f; // fuerza del salto
        isJumping = true;
    }
}

//collion
void Player::Update(Scene& scene) {

    int tileSize = scene.GetTileSize();

    // HORIZONTAL

    float nextX = position.x + moveX * speed;

    int leftTile = (int)(nextX / tileSize);
    int rightTile = (int)((nextX + texture.width * scale - 1) / tileSize);
    int topTile = (int)(position.y / tileSize);
    int bottomTile = (int)((position.y + texture.height * scale - 1) / tileSize);

    if (moveX < 0) {
        if (scene.IsSolid(leftTile, topTile) || scene.IsSolid(leftTile, bottomTile)) {
            nextX = (float) (leftTile + 1) * tileSize;
        }
    }
    else if (moveX > 0) {
        if (scene.IsSolid(rightTile, topTile) || scene.IsSolid(rightTile, bottomTile)) {
            nextX = (float) (rightTile * tileSize) - texture.width * scale;
        }
    }

    position.x = nextX;

    // VERTICAL

    velocityY += gravity;
    float nextY = position.y + velocityY;

    leftTile = (int)(position.x / tileSize);
    rightTile = (int)((position.x + texture.width * scale) / tileSize);
    topTile = (int)(nextY / tileSize);
    bottomTile = (int)((nextY + texture.height * scale) / tileSize);

    if (velocityY > 0 &&
        (scene.IsSolid(leftTile, bottomTile) || scene.IsSolid(rightTile, bottomTile))) {

        position.y = (float) bottomTile * tileSize - texture.height * scale;
        velocityY = 0;
        isJumping = false;
    }
    else if (velocityY < 0 &&
        (scene.IsSolid(leftTile, topTile) || scene.IsSolid(rightTile, topTile))) {

        position.y = (float) (topTile + 1) * tileSize;
        velocityY = 0;
    }
    else {
        position.y = nextY;
    }
}

void Player::Draw() {
    DrawTextureEx(texture,position,0.0f,scale,WHITE);
    DrawRectangleLines(
        position.x,
        position.y,
        texture.width * scale,
        texture.height * scale,
        RED
    );
} 