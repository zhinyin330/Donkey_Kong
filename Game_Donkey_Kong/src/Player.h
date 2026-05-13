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
    HAMMER_SWING  //挥锤状态
};

// ========== 新增：浮动文字结构 ==========
struct FloatingText {
    Vector2 position;
    std::string text;
    float lifetime;
    float alpha;
    float floatOffset;  // 向上飘的偏移量

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

    //锤子
    std::vector<Texture2D> hammerSwingTextures;  // 挥锤动画
    bool hasHammer;
    bool isSwingingHammer;
    float swingHammerTimer;
    float swingHammerDuration;
    int swingFrame;
    Vector2 hammerOffset;  // 锤子相对于玩家的偏移

    // Audio
    Sound jumpSound;
    Sound walkSound;
    float stepTimer;
    float stepInterval;
    bool wasMovingLeft;
    bool wasMovingRight;

    bool wasMoving;
    bool isStepPlaying;

    float climbStepInterval; // 爬梯子间隔

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
    int lives;

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

   
    int score; //得分
    
    std::vector<FloatingText> floatingTexts;// 浮动加分列表

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

    // ========== 新增：浮动文字方法 ==========
    void AddFloatingText(Vector2 worldPos, const std::string& text, int points);
    void UpdateFloatingTexts();
    void DrawFloatingTexts();
    // 获取玩家脚下位置（用于显示分数飘字）
    Vector2 GetFeetWorldPos() const {
        return { position.x + currentTexture.width * scale / 2,
                 position.y + (baseHitboxOffsetY + baseHitboxHeight) * scale };
    }

    // Temporal: Hitbox para colisiones
    Rectangle GetHitbox() const {
        return {
            position.x,
            position.y + baseHitboxOffsetY * scale,
            currentTexture.width * scale,
            baseHitboxHeight * scale
        };
    }

    //vidas
    void Respawn(int tileX, int tileY);
    int GetLives() const { return lives; }
    void LoseLife() { if (lives > 0) lives--; }
    bool IsDead() const { return lives <= 0; }

    //锤子系统方法
 // ========== 在这里添加锤子动画纹理加载 ==========
    void SetHasHammer(bool has) { hasHammer = has; }
    bool HasHammer() const { return hasHammer; }
    void StartSwingHammer();
    bool IsSwingingHammer() const { return isSwingingHammer; }
    void UpdateHammerSwing(float deltaTime);
    Rectangle GetAttackHitbox() const;
    // ========== 锤子纹理加载结束 ==========
};