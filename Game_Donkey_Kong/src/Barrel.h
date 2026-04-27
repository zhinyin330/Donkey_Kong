#pragma once
#include "raylib.h"

enum class BarrelType {
    NORMAL,
    BLUE_BARREL
};

class Barrel {
public:
    BarrelType type;
    Vector2 position;
    float speed;

    Barrel(BarrelType t, Vector2 pos) {
        type = t;
        position = pos;
        speed = 100.0f;
    }

    void Update() {
        position.x += speed * GetFrameTime(); // 简单向右滚
    }

    void Draw() {
        Color color = (type == BarrelType::NORMAL) ? BROWN : BLUE;
        DrawCircleV(position, 10, color); //  用圆代替贴图（先跑起来）
    }
};