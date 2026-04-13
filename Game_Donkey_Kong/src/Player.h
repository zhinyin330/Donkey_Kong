#pragma once
#include "raylib.h"
#include <vector>

class Scene;

enum class PlayerState {
    IDLE,
    WALKING,
    WALK_END,
    JUMPING
};

class Player {
private:
    // Texturas
    Texture2D idleTexture;
    std::vector<Texture2D> walkTextures;
    Texture2D walkEndTexture;
    Texture2D jumpTexture;
    Texture2D currentTexture;

    //Audio
    Sound jumpSound;

    // Animación
    PlayerState currentState;
    int currentFrame;
    float frameCounter;
    float frameSpeed;
    int walkEndCounter;

    Vector2 position;
    float speed;
    float velocityY;
    float gravity;
    bool isJumping;
    float scale;
    float moveX;
    bool facingRight;

    // Hitbox base (valores para IDLE)
    int baseHitboxOffsetY;
    int baseHitboxHeight;

    // Ajustes adicionales por estado
    int jumpHitboxOffsetY;   // Offset extra para salto
    int jumpHitboxHeight;    // Altura específica para salto

    //Método para obtener la posición de los pies
    float GetFeetPosition();
public:
    Player();
    ~Player();

    void HandleInput();
    void Update(Scene& scene);
    void Draw();
    void UpdateAnimation();
    void ChangeState(PlayerState newState);

    // Obtener hitbox actual según el estado
    void SetFeetPosition(float feetY);
    int GetCurrentHitboxOffsetY();
    int GetCurrentHitboxHeight();
    float GetCollisionHeight() { return GetCurrentHitboxHeight() * scale; };
};