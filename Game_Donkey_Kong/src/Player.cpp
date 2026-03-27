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

void Player::Update() {
    // Gravedad
    velocityY += gravity;
    position.y += velocityY;

    //Suelo
    float groundLevel = 300.0f;
    if (position.y >= groundLevel) {
        position.y = groundLevel;
        velocityY = 0.0f;
        isJumping = false;
    }
}

void Player::Draw() {
    DrawTexture(texture, (int)position.x, (int)position.y, WHITE);
}