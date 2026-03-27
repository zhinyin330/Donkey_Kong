#pragma once
#include "raylib.h"

class Player {

private:
    Texture2D texture;
    Vector2 position;

    float speed;

    //Gravedad
    float velocityY;
    float gravity;
    bool isJumping;

public:
    Player();

    void HandleInput();
    void Update();
    void Draw();


};