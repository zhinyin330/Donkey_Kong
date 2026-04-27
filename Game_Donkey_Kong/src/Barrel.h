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
    FALLING
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

    private:
        BarrelType type;
        BarrelState state;

        Vector2 position;
        float speed;
        bool movingRight;

        float groundOffset; // 贴地修正

        // ===== 动画 =====
        std::vector<Texture2D> frames;
        int currentFrame;
        float frameCounter;
        float frameSpeed;
};