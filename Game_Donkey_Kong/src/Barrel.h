#pragma once
#include "raylib.h"
#include <vector>

class GameScene;

enum class BarrelType {
    NORMAL,
    BLUE_BARREL
};

enum class BarrelState {
    ROLLING,      // 水平滚动
    FALLING,      // 普通掉落
    VERTICAL_DROP // 指定位置纵向下落
};

struct VerticalDropTrigger
{
    float x;
    float y;
};

class Barrel {
    public:
        // 构造函数
        Barrel(BarrelType t, Vector2 pos);

        // 更新 / 渲染
        void Update(GameScene& scene);
        void Draw();

        void Hit();           // 桶被锤子打中时调用
        bool IsHit() const { return isHit; }  // 检查桶是否已被打中
        BarrelType GetType() const { return type; }
    private:
        void UpdateAnimation();
        void LoadFrames();  //加载动画帧的方法
        // 检测是否触发纵向下落
        bool CheckVerticalDropTrigger();

private:
    BarrelType type;
    BarrelState state;

        Vector2 position;
        float speed;

        // 纵向下落速度
        float verticalDropSpeed;
        bool movingRight;

        int targetPlatformY;

        float groundOffset; // 地面贴合偏移
        // ===== 动画 =====
        std::vector<Texture2D> frames;
        int currentFrame;
        float frameCounter;
        float frameSpeed;

        bool isHit = false;  // 标记桶是否被锤子打中（打中后不再移动，用于加分后消失）

        // ===== 动画分组 =====
        //  以下4个为新增
        std::vector<Texture2D> rollingFrames;      // 普通滚动动画帧
        std::vector<Texture2D> fallingFrames;      // 掉落动画帧
        std::vector<Texture2D> currentAnimationFrames;  // 当前使用的动画组

        // 调试触发点
        std::vector<VerticalDropTrigger> verticalDropTriggers;

    public:
        // 以下3个为新增
        Vector2 GetPosition() const { return position; }
        float GetWidth() const
        {
            return rollingFrames.empty() ? 0 : rollingFrames[0].width * 2.5f;
        }

        float GetHeight() const
        {
            return rollingFrames.empty() ? 0 : rollingFrames[0].height * 2.5f;
        }
};