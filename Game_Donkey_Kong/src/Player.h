#pragma once
#include "raylib.h"

class Player {
public:
    Player();

    void HandleInput();
    void Update();
    void Draw();

private:
    Texture2D texture;
    Vector2 position;

    float speed;
};