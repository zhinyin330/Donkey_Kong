#pragma once
#include "raylib.h"
#include <vector>

// 火人动画类型
enum class FireAnimationType {
    SCENE1,    
    SCENE2      
};

class FireSprite
{
private:

    //  动画帧  
    std::vector<Texture2D> frames;
    int currentFrame = 0;
    float frameTimer = 0.0f;
    float frameDelay = 0.12f;  //降低动画速度，减少闪烁感（原0.15f）
    float frameAccumulator = 0.0f;  //累积时间用于更平滑的动画

    //  位置  
    Vector2 position;

    //  跳跃移动系统  
    enum JumpState {
        IDLE,           // 待机状态（落地后短暂停留）
        JUMPING
    };

    JumpState jumpState = IDLE;

    // 跳跃参数
    float jumpStartX;           // 跳跃起始X坐标
    float jumpStartY;           // 跳跃起始Y坐标（地面Y）
    float jumpTargetX;          // 跳跃目标X坐标
    float jumpProgress;         // 跳跃进度 (0.0 ~ 1.0)
    float jumpDuration = 0.4f;  // 每次跳跃总时长（秒）
    float jumpTimer = 0.0f;     // 当前跳跃计时器

    float jumpHeight = 12.0f;   // 跳跃高度（像素）
    float jumpDistance = 45.0f; // 每次跳跃移动的距离（像素）
    // 地面Y坐标
    float groundY;

    //  移动范围  
    float minX, maxX;

    //  方向（-1向左，1向右） 
    int direction = 1;
    int lastDirection = 1;

    //  落地后等待计时器  
    float idleTimer = 0.0f;
    float idleDuration = 0.25f;  // 落地后等待时间（秒）

    // 是否激活  
    bool isActive = true;

    // 是否死亡  
    bool isDead = false;

    //   等待系统（生成后原地等待2秒） 
    bool isWaiting = true;       // 是否在等待状态
    float waitTimer = 2.0f;      // 等待计时器（2秒）
    float blinkTimer = 0.0f;     // 闪烁计时器
    bool visible = true;         // 当前是否可见（用于闪烁效果）

    // 动画类型
    FireAnimationType animationType;
    // 根据类型加载贴图
    void LoadFramesByType();

    //   辅助方法  
    void StartNewJump();
    void UpdateAnimation(float deltaTime);

    // 计算平滑的抛物线位置
    Vector2 CalculateParabolaPosition(float t);

    // ===== 新增：爆炸飞出状态 =====
    bool isFlyingFromOilCan = false;  // 是否正在从油桶飞出
    float flyProgress = 0.0f;         // 飞出进度
    float flyDuration = 0.3f;         // 飞出持续时间
    Vector2 flyStartPos;              // 飞出起始位置（油桶位置）
    Vector2 flyTargetPos;             // 飞出目标位置（最终站立位置）
    float flyArcHeight = 80.0f;       // 飞出弧线高度


public:
    // 构造函数：默认使用 SCENE1 类型
    FireSprite(Vector2 startPos, FireAnimationType type = FireAnimationType::SCENE1);
    ~FireSprite();

    void Update(float deltaTime);
    void Draw();
    Rectangle GetHitbox();

    //  设置移动范围  
    void SetRange(float minX, float maxX);

    //  获取位置  
    Vector2 GetPosition() const { return position; }
    //  激活/禁用小火人  
    void SetActive(bool active) { isActive = active; }
    bool IsActive() const { return isActive; }

    //  设置地面Y坐标  
    void SetGroundY(float y) { groundY = y; }

    //  重置位置（用于重生） 
    void ResetPosition(Vector2 newPos);

    //  小火人死亡  
    void Die();
    bool IsDead() const { return isDead; }
    // 获取是否可以对玩家造成伤害
    bool CanHurt() const { return !isWaiting; }

    float GetGroundY() const { return groundY; }
    void SetPosition(Vector2 pos) { position = pos; }
    bool IsWaiting() const { return isWaiting; }

    // ===== 新增：从油桶飞出到目标位置 =====
    void StartFlyFromOilCan(Vector2 startPos, Vector2 targetPos);
    bool IsFlying() const { return isFlyingFromOilCan; }
};