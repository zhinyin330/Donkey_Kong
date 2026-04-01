#pragma once
#include "raylib.h"

class Scene {

private:
    static const int tileSize = 16;
    static const int mapWidth = 620 / tileSize;
    static const int mapHeight = 700 / tileSize;

    int level[mapHeight][mapWidth];

    Texture2D tileTexture;

public:
    Scene();

    void Draw();
    bool IsSolid(int x, int y);
    int GetTileSize();
};