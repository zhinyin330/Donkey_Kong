#pragma once
#include "raylib.h"

class Scene; // declaracion adelantada (collision)

class Player {

private:
    Texture2D texture;
    Vector2 position;

    float speed;

    //Gravedad
    float velocityY;
    float gravity;
    bool isJumping;

    float scale;

public:
    Player();

    void HandleInput();
    void Update(Scene& scene);
    void Draw();
};
