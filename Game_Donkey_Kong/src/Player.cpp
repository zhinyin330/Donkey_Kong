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

    float nextX = position.x;
    float nextY = position.y;

    int leftTile = (int)(nextX / tileSize);
    int rightTile = (int)((nextX + texture.width * scale) / tileSize);
    int topTile = (int)(position.y / tileSize);
    int bottomTile = (int)((position.y + texture.height * scale) / tileSize);

    // colisión lateral
    if (scene.IsSolid(leftTile, topTile) || scene.IsSolid(leftTile, bottomTile) ||
        scene.IsSolid(rightTile, topTile) || scene.IsSolid(rightTile, bottomTile)) {

        // bloquea movimiento
        nextX = position.x;
    }

    position.x = nextX;


    // MOVIMIENTO VERTICAL
    velocityY += gravity;
    nextY = position.y + velocityY;

    topTile = (int)(nextY / tileSize);
    bottomTile = (int)((nextY + texture.height * scale) / tileSize);
    leftTile = (int)(position.x / tileSize);
    rightTile = (int)((position.x + texture.width * scale) / tileSize);

    //COLISIÓN ABAJO (suelo)
    if (velocityY > 0 &&
        (scene.IsSolid(leftTile, bottomTile) || scene.IsSolid(rightTile, bottomTile))) {

        position.y = bottomTile * tileSize - texture.height * scale;
        velocityY = 0;
        isJumping = false;
    }
    //COLISIÓN ARRIBA (techo)
    else if (velocityY < 0 &&
        (scene.IsSolid(leftTile, topTile) || scene.IsSolid(rightTile, topTile))) {

        position.y = (topTile + 1) * tileSize;
        velocityY = 0;
    }
    else {
        position.y = nextY;
    }
}

void Player::Draw() {
    DrawTextureEx(texture,position,0.0f,scale,WHITE);
}