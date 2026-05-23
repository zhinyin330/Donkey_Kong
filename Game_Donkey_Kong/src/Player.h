#pragma once
#include "raylib.h"
#include <vector>
#include <string>

class GameScene;

enum class PlayerState {
    IDLE,
    WALKING,
    WALK_END,
    JUMPING,
    CLIMBING,
    CLIMBING_END,
    HAMMER_SWING 
};

struct FloatingText {
    Vector2 position;
    std::string text;
    float lifetime;
    float alpha;

    float floatOffset;  


    FloatingText(Vector2 pos, const std::string& txt, float duration = 1.0f)
        : position(pos), text(txt), lifetime(duration), alpha(1.0f), floatOffset(0.0f) {}
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
    Texture2D deathEndTexture;
    std::vector<Texture2D> deathTextures;
    std::vector<Rectangle> jumpedBarrels;
    std::vector<Texture2D> hammerSwingTextures; 
    Texture2D starIconTexture;
    Texture2D hammerIconTexture;


    bool hasHammer;
    bool isSwingingHammer;
    float swingHammerTimer;
    float swingHammerDuration;
    int swingFrame;
    Vector2 hammerOffset; 

    // Audio
    Sound jumpSound;
    Sound walkSound;
    float stepTimer;
    float stepInterval;
    bool wasMovingLeft;
    bool wasMovingRight;
    // 音效
    Sound deathSound;
    bool deathSoundLoaded;

    bool wasMoving;
    bool isStepPlaying;

    float climbStepInterval; 

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
    float velocityX;
    float moveY;
    bool onLadder;
    bool isClimbing;
    float climbSpeed;
    bool exitingLadder;
    bool wasInAir;

    static int lives;
    bool isDying;
    float deathTimer;
    int deathFrame;

    // Hitbox base
    int baseHitboxOffsetY;
    int baseHitboxHeight;

    // Ajustes por estado
    int jumpHitboxOffsetY;
    int jumpHitboxHeight;

    // Métodos privados
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

    // 锤子冷却系统
    float hammerCooldownTimer;    
    float hammerCooldownDuration;  
    bool isHammerOnCooldown;    
 
    float displayTimeLeft = 120.0f;

    int score; 
    
    std::vector<FloatingText> floatingTexts;

public:
    Player();
    ~Player();

    void SetScore(int score);

    // Usar GameScene& en lugar de Scene&
    void HandleInput(GameScene& scene);
    void Update(GameScene& scene);
    void Draw();
    void UpdateAnimation();
    void ChangeState(PlayerState newState);

    void SetHasHammer(bool has);
    bool HasHammer() const { return hasHammer; }

    // Getters públicos
    Vector2 GetPosition() const { return position; }
    float GetVelocityY() const { return velocityY; }
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

    bool IsHammerOnCooldown() const { return isHammerOnCooldown; }
    float GetHammerCooldownRemaining() const {
        return hammerCooldownTimer > 0 ? hammerCooldownTimer : 0;
    }

    void AddScore(int points);  
    int GetScore() const { return score; }  
    void ResetScore(); 

   
    void AddFloatingText(Vector2 worldPos, const std::string& text, int points);
    void UpdateFloatingTexts();
    void DrawFloatingTexts();
  
    Vector2 GetFeetWorldPos() const {
        return { position.x + currentTexture.width * scale / 2,
                 position.y + (baseHitboxOffsetY + baseHitboxHeight) * scale };
    }

    // Hitbox para colisiones
    Rectangle GetHitbox() const {
        return {
        position.x,
        position.y + baseHitboxOffsetY * scale,
        currentTexture.width * scale,
        baseHitboxHeight * scale
        };
    }
    float justLandedTimer = 0.0f;
    bool HasJustLanded() const { return justLandedTimer > 0.0f; }
    void ClearJustLanded() { justLandedTimer = 0.0f; }

    //vidas
    void Respawn(int tileX, int tileY);
    int GetLives() const { return lives; }
    void LoseLife() { if (lives > 0) lives--; }
    bool IsDead() const { return lives <= 0; }
    bool IsDying() const { return isDying; }
    void StartDeath();
    void UpdateDeath(float deltaTime);
    static void ResetLives() { lives = 3; }

  
    void StartSwingHammer();
    bool IsSwingingHammer() const { return isSwingingHammer; }
    void UpdateHammerSwing(float deltaTime);
    Rectangle GetAttackHitbox() const;

    void SetTimeLeft(float t) { displayTimeLeft = t; }
  
    // 播放死亡音效
    void PlayDeathSound();
};