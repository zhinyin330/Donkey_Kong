#pragma once
#include "raylib.h"

class Scene {

private:
    static const int tileSize = 16;
    static const int tileScale = 2;
    static const int mapWidth = 25;
    static const int mapHeight = 18;

    int level[mapHeight][mapWidth];

    Texture2D tileTexture;

public:

    static int GetScreenWidth() { return mapWidth * tileSize * tileScale; }
    static int GetScreenHeight() { return mapHeight * tileSize * tileScale; }

    Scene();

    void Draw();
    bool IsSolid(int x, int y);
    int GetTileSize() { return tileSize * tileScale; }
};