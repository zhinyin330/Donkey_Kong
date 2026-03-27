#pragma once
#include "raylib.h"

class Scene {

private:
    static constexpr float tileSize = 620.0f / 14.0f;
    static const int mapWidth = 14;
    static const int mapHeight = 700/ tileSize;

    int level[mapHeight][mapWidth];

    Texture2D tileTexture;

    // collision
    bool IsSolid(int x, int y);
    int GetTileSize();

public:
    Scene();

    void Draw();
};