#pragma once
#include "raylib.h"
#include <vector>

class GameScene;

enum class BarrelType {
    NORMAL,
    BLUE_BARREL
};

enum class BarrelState {
    ROLLING,
    ROLLING_DOWN_LADDER,  //沿梯子向下滚动状态
    FALLING,
    FALLING_DOWN_LADDER
};

class Barrel {
    public:
        // 构造函数
        Barrel(BarrelType t, Vector2 pos);

        // 更新 / 渲染
        void Update(GameScene& scene);
        void Draw();
    private:
        void UpdateAnimation();
        void LoadFrames();  //加载动画帧的方法

    private:
        BarrelType type;
        BarrelState state;

        Vector2 position;
        float speed;
        bool movingRight;

        float groundOffset; // 贴地修正

        int targetPlatformY;      // 目标平台的Y坐标
        int FindNextPlatformBelow(GameScene& scene, int ladderX, int startY);  // 辅助函数

        // ===== 动画 =====
        std::vector<Texture2D> frames;
        int currentFrame;
        float frameCounter;
        float frameSpeed;

        float ladderRollSpeed;  //：沿梯子滚动的速度

        // ===== 动画分组 =====
        //  以下4个为新增
        std::vector<Texture2D> rollingFrames;      // 普通滚动动画帧
        std::vector<Texture2D> fallingFrames;      // 掉落动画帧
        std::vector<Texture2D> ladderFrames;       // 梯子滚动动画帧
        std::vector<Texture2D> currentAnimationFrames;  // 当前使用的动画组

        //  梯子滚动状态记录
        int currentLadderX;
        int currentLadderY;
        float ladderProgress;

    public:
        // 以下3个为新增
        Vector2 GetPosition() const { return position; }
        float GetWidth() const { return frames.empty() ? 0 : frames[0].width * 2.5f; }
        float GetHeight() const { return frames.empty() ? 0 : frames[0].height * 2.5f; }
};