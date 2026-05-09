#pragma once
#include "raylib.h"
#include <vector>

class GameScene;

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
    std::vector<Texture2D> climbTextures;
    std::vector<Texture2D> climbEndTextures;

    // Audio
    Sound jumpSound;
    Sound walkSound;
    float stepTimer;
    float stepInterval;

    bool wasMoving;
    bool isStepPlaying;

    // Animaci髇
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
    float velocityX;
    float moveY;
    bool onLadder;
    bool isClimbing;
    float climbSpeed;
    bool exitingLadder;

    // Hitbox base
    int baseHitboxOffsetY;
    int baseHitboxHeight;

    // Ajustes por estado
    int jumpHitboxOffsetY;
    int jumpHitboxHeight;

    // M閠odos privados
    float GetFeetPosition();
    void SetFeetPosition(float feetY);
    int GetCurrentHitboxOffsetY();
    int GetCurrentHitboxHeight();

    // Star
    int starCount;
    static const int maxStars = 10;

    // Modo estrella
    bool starMode;
    float starModeTimer;
    static const float starModeDuration;
    Color currentTint;

    //得分
    int score;
public:
    Player();
    ~Player();

    // Usar GameScene& en lugar de Scene&
    void HandleInput(GameScene& scene);
    void Update(GameScene& scene);
    void Draw();
    void UpdateAnimation();
    void ChangeState(PlayerState newState);

    // Getters p鷅licos
    Vector2 GetPosition() const { return position; }
    float GetScale() const { return scale; }
    float GetTextureWidth() const { return currentTexture.width; }
    float GetTextureHeight() const { return currentTexture.height; }
    float GetCollisionHeight() { return GetCurrentHitboxHeight() * scale; }

    // Star
    void AddStar() { if (starCount < maxStars) starCount++; }
    int GetStarCount() const { return starCount; }
    int GetMaxStars() const { return maxStars; }
    bool HasMaxStars() const { return starCount >= maxStars; }

    // Modo estrella
    void ActivateStarMode();
    void UpdateStarMode();
    bool IsInStarMode() const { return starMode; }

    //得分
    void AddScore(int points);  
    int GetScore() const { return score; }  
    void ResetScore(); 

    // Temporal: Hitbox para colisiones
    Rectangle GetHitbox() const {
        return {
            position.x,
            position.y + baseHitboxOffsetY * scale,
            currentTexture.width * scale,
            baseHitboxHeight * scale
        };
    }
};