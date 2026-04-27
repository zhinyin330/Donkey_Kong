#pragma once
#include "raylib.h"
#include <vector>

class Scene;

enum class PlayerState {
    IDLE,
    WALKING,
    WALK_END,
    JUMPING,
    CLIMBING,
    CLIMBING_END
};

class Player {
private:
    // Texturas
    Texture2D idleTexture;
    std::vector<Texture2D> walkTextures;
    Texture2D walkEndTexture;
    Texture2D jumpTexture;
    Texture2D currentTexture;
    std::vector<Texture2D> climbTextures;      // Ladder1, Ladder2
    std::vector<Texture2D> climbEndTextures;   // LadderEnd1, LadderEnd2

    //Audio
    Sound jumpSound;
    //walking audio
    Sound walkSound;
    float stepTimer;
    float stepInterval;

    bool wasMoving;             
    bool isStepPlaying;         

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
    float velocityX; // inercia horizontal
    float moveY;
    bool onLadder;
    bool isClimbing;
    float climbSpeed;
    bool exitingLadder;

    // Hitbox base (valores para IDLE)
    int baseHitboxOffsetY;
    int baseHitboxHeight;

    // Ajustes adicionales por estado
    int jumpHitboxOffsetY;   // Offset extra para salto
    int jumpHitboxHeight;    // Altura específica para salto

    //Método para obtener la posición de los pies
    float GetFeetPosition();

    //Star
    int starCount;      
    const int maxStars = 10;
public:
    Player();
    ~Player();

    void HandleInput(Scene& scene);
    void Update(Scene& scene);
    void Draw();
    void UpdateAnimation();
    void ChangeState(PlayerState newState);

    // Obtener hitbox actual según el estado
    void SetFeetPosition(float feetY);
    int GetCurrentHitboxOffsetY();
    int GetCurrentHitboxHeight();
    float GetCollisionHeight() { return GetCurrentHitboxHeight() * scale; };

    //start
    Vector2 GetPosition() const { return position; }
    float GetScale() const { return scale; }
    float GetTextureWidth() const { return currentTexture.width; }
    float GetTextureHeight() const { return currentTexture.height; }
    void AddStar() { if (starCount < maxStars) starCount++; }
    int GetStarCount() const { return starCount; }
    int GetMaxStars() const { return maxStars; }
    bool HasMaxStars() const { return starCount >= maxStars; }
};