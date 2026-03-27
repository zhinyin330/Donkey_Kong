#include "Player.h"
#include "resource_dir.h" 

Player::Player() {
    texture = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png"); // tu sprite en reposo

    position = { 100.0f, 300.0f };
    speed = 4.0f;
}

void Player::HandleInput() {
    if (IsKeyDown(KEY_A)) {
        position.x -= speed;
    }
    if (IsKeyDown(KEY_D)) {
        position.x += speed;
    }
}

void Player::Update() {
    // De momento vacío
}

void Player::Draw() {
    DrawTexture(texture, (int)position.x, (int)position.y, WHITE);
}